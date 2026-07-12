#include "audio_service.h"
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <esp_system.h>
#include <cstring>
#include <cstdlib>

#define RATE_CVT_CFG(_src_rate, _dest_rate, _channel)        \
    (esp_ae_rate_cvt_cfg_t)                                  \
    {                                                        \
        .src_rate        = (uint32_t)(_src_rate),            \
        .dest_rate       = (uint32_t)(_dest_rate),           \
        .channel         = (uint8_t)(_channel),              \
        .bits_per_sample = ESP_AUDIO_BIT16,                  \
        .complexity      = 2,                                \
        .perf_type       = ESP_AE_RATE_CVT_PERF_TYPE_SPEED,  \
    }

#define OPUS_DEC_CFG(_sample_rate, _frame_duration_ms)                                                    \
    (esp_opus_dec_cfg_t)                                                                                  \
    {                                                                                                     \
        .sample_rate    = (uint32_t)(_sample_rate),                                                       \
        .channel        = ESP_AUDIO_MONO,                                                                 \
        .frame_duration = (esp_opus_dec_frame_duration_t)AS_OPUS_GET_FRAME_DRU_ENUM(_frame_duration_ms),  \
        .self_delimited = false,                                                                          \
    }

#if CONFIG_USE_AUDIO_PROCESSOR
#include "processors/afe_audio_processor.h"
#else
#include "processors/no_audio_processor.h"
#endif

#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32P4
#include "wake_words/afe_wake_word.h"
#include "wake_words/custom_wake_word.h"
#else
#include "wake_words/esp_wake_word.h"
#endif

#define TAG "AudioService"

namespace {
// Realtime call setup on ESP32-C5 needs more headroom than the old 12x stack,
// but 16x pushes idle free SRAM too low for MQTT/TLS on this board.
constexpr uint32_t kOpusCodecTaskStackBytes = 2048 * 14;
constexpr uint32_t kOpusCodecStackLogInterval = 20;
constexpr size_t kMinHeapForSoundPlayback = 20 * 1024;
constexpr size_t kMinLargestBlockForSoundPlayback = 16 * 1024;
constexpr size_t kMinHeapForSoundQueuePacket = 18 * 1024;
constexpr size_t kMinLargestBlockForSoundQueuePacket = 14 * 1024;
#if CONFIG_IDF_TARGET_ESP32C5
constexpr size_t kMinHeapForWakeWordInit = 24 * 1024;
constexpr size_t kMinLargestBlockForWakeWordInit = 18 * 1024;
#endif
}

AudioService::AudioService() {
    event_group_ = xEventGroupCreate();
}

AudioService::~AudioService() {
    if (event_group_ != nullptr) {
        vEventGroupDelete(event_group_);
    }
    if (opus_encoder_ != nullptr) {
        esp_opus_enc_close(opus_encoder_);
    }
    if (opus_decoder_ != nullptr) {
        esp_opus_dec_close(opus_decoder_);
    }
    if (input_resampler_ != nullptr) {
        esp_ae_rate_cvt_close(input_resampler_);
    }
    if (output_resampler_ != nullptr) {
        esp_ae_rate_cvt_close(output_resampler_);
    }
}

void AudioService::Initialize(AudioCodec* codec) {
    codec_ = codec;
    codec_->Start();

    esp_opus_dec_cfg_t opus_dec_cfg = OPUS_DEC_CFG(codec->output_sample_rate(), OPUS_FRAME_DURATION_MS);
    auto ret = esp_opus_dec_open(&opus_dec_cfg, sizeof(esp_opus_dec_cfg_t), &opus_decoder_);
    if (opus_decoder_ == nullptr) {
        ESP_LOGE(TAG, "Failed to create audio decoder, error code: %d", ret);
    } else {
        decoder_sample_rate_ = codec->output_sample_rate();
        decoder_duration_ms_ = OPUS_FRAME_DURATION_MS;
        decoder_frame_size_ = decoder_sample_rate_ / 1000 * OPUS_FRAME_DURATION_MS;
        ESP_LOGI(TAG, "Opus decoder ready: %d Hz, %d ms, frame_samples=%d",
                 decoder_sample_rate_, decoder_duration_ms_, decoder_frame_size_);
    }
    esp_opus_enc_config_t opus_enc_cfg = AS_OPUS_ENC_CONFIG();
    ret = esp_opus_enc_open(&opus_enc_cfg, sizeof(esp_opus_enc_config_t), &opus_encoder_);
    if (opus_encoder_ == nullptr) {
        ESP_LOGE(TAG, "Failed to create audio encoder, error code: %d", ret);
    } else {
        encoder_sample_rate_ = 16000;
        encoder_duration_ms_ = OPUS_FRAME_DURATION_MS;
        esp_opus_enc_get_frame_size(opus_encoder_, &encoder_frame_size_, &encoder_outbuf_size_);
        encoder_frame_size_ = encoder_frame_size_ / sizeof(int16_t);
        ESP_LOGI(TAG, "Opus encoder ready: %d Hz, %d ms, frame_samples=%u, outbuf=%u",
                 encoder_sample_rate_, encoder_duration_ms_, encoder_frame_size_, encoder_outbuf_size_);
    }

    if (codec->input_sample_rate() != 16000) {
        esp_ae_rate_cvt_cfg_t input_resampler_cfg = RATE_CVT_CFG(
            codec->input_sample_rate(), ESP_AUDIO_SAMPLE_RATE_16K, codec->input_channels());
        auto resampler_ret = esp_ae_rate_cvt_open(&input_resampler_cfg, &input_resampler_);
        if (input_resampler_ == nullptr) {
            ESP_LOGE(TAG, "Failed to create input resampler, error code: %d", resampler_ret);
        }
    }

#if CONFIG_USE_AUDIO_PROCESSOR
    audio_processor_ = std::make_unique<AfeAudioProcessor>();
#else
    audio_processor_ = std::make_unique<NoAudioProcessor>();
#endif

    audio_processor_->OnOutput([this](std::vector<int16_t>&& data) {
        PushTaskToEncodeQueue(kAudioTaskTypeEncodeToSendQueue, std::move(data));
    });

    audio_processor_->OnVadStateChange([this](bool speaking) {
        voice_detected_ = speaking;
        if (callbacks_.on_vad_change) {
            callbacks_.on_vad_change(speaking);
        }
    });

    esp_timer_create_args_t audio_power_timer_args = {
        .callback = [](void* arg) {
            AudioService* audio_service = (AudioService*)arg;
            audio_service->CheckAndUpdateAudioPowerState();
        },
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "audio_power_timer",
        .skip_unhandled_events = true,
    };
    esp_timer_create(&audio_power_timer_args, &audio_power_timer_);
}

void AudioService::Start() {
    service_stopped_ = false;
    xEventGroupClearBits(event_group_, AS_EVENT_AUDIO_TESTING_RUNNING | AS_EVENT_WAKE_WORD_RUNNING | AS_EVENT_AUDIO_PROCESSOR_RUNNING);

    esp_timer_start_periodic(audio_power_timer_, 1000000);

#if CONFIG_USE_AUDIO_PROCESSOR
    /* Start the audio input task */
    xTaskCreatePinnedToCore([](void* arg) {
        AudioService* audio_service = (AudioService*)arg;
        audio_service->AudioInputTask();
        vTaskDelete(NULL);
    }, "audio_input", 2048 * 3, this, 8, &audio_input_task_handle_, 0);

    /* Start the audio output task */
    xTaskCreate([](void* arg) {
        AudioService* audio_service = (AudioService*)arg;
        audio_service->AudioOutputTask();
        vTaskDelete(NULL);
    }, "audio_output", 2048 * 2, this, 4, &audio_output_task_handle_);
#else
    /* Start the audio input task */
    xTaskCreate([](void* arg) {
        AudioService* audio_service = (AudioService*)arg;
        audio_service->AudioInputTask();
        vTaskDelete(NULL);
    }, "audio_input", 2048 * 2, this, 8, &audio_input_task_handle_);

    /* Start the audio output task */
    xTaskCreate([](void* arg) {
        AudioService* audio_service = (AudioService*)arg;
        audio_service->AudioOutputTask();
        vTaskDelete(NULL);
    }, "audio_output", 2048, this, 4, &audio_output_task_handle_);
#endif

    /* Start the opus codec task */
    xTaskCreate([](void* arg) {
        AudioService* audio_service = (AudioService*)arg;
        audio_service->OpusCodecTask();
        vTaskDelete(NULL);
    }, "opus_codec", kOpusCodecTaskStackBytes, this, 2, &opus_codec_task_handle_);
}

void AudioService::Stop() {
    esp_timer_stop(audio_power_timer_);
    service_stopped_ = true;
    xEventGroupSetBits(event_group_, AS_EVENT_AUDIO_TESTING_RUNNING |
        AS_EVENT_WAKE_WORD_RUNNING |
        AS_EVENT_AUDIO_PROCESSOR_RUNNING);

    std::lock_guard<std::mutex> lock(audio_queue_mutex_);
    audio_encode_queue_.clear();
    audio_decode_queue_.clear();
    audio_playback_queue_.clear();
    audio_testing_queue_.clear();
    audio_queue_cv_.notify_all();
}

bool AudioService::ReadAudioData(std::vector<int16_t>& data, int sample_rate, int samples) {
    if (!codec_->input_enabled()) {
        esp_timer_stop(audio_power_timer_);
        esp_timer_start_periodic(audio_power_timer_, AUDIO_POWER_CHECK_INTERVAL_MS * 1000);
        codec_->EnableInput(true);
    }

    if (codec_->input_sample_rate() != sample_rate) {
        data.resize(samples * codec_->input_sample_rate() / sample_rate * codec_->input_channels());
        if (!codec_->InputData(data)) {
            return false;
        }
        if (input_resampler_ != nullptr) {
            std::lock_guard<std::mutex> lock(input_resampler_mutex_);
            uint32_t in_sample_num = data.size() / codec_->input_channels();
            uint32_t output_samples = 0;
            esp_ae_rate_cvt_get_max_out_sample_num(input_resampler_, in_sample_num, &output_samples);
            auto resampled = std::vector<int16_t>(output_samples * codec_->input_channels());
            uint32_t actual_output = output_samples;
            esp_ae_rate_cvt_process(input_resampler_, (esp_ae_sample_t)data.data(), in_sample_num,
                                   (esp_ae_sample_t)resampled.data(), &actual_output);
            resampled.resize(actual_output * codec_->input_channels());
            data = std::move(resampled);
        }
    } else {
        data.resize(samples * codec_->input_channels());
        if (!codec_->InputData(data)) {
            return false;
        }
    }

    /* Update the last input time */
    last_input_time_ = std::chrono::steady_clock::now();
    debug_statistics_.input_count++;

    if ((debug_statistics_.input_count % 100) == 0) {
        int max_abs = 0;
        for (auto sample : data) {
            int value = std::abs((int)sample);
            if (value > max_abs) {
                max_abs = value;
            }
        }
        ESP_LOGI(TAG, "Mic PCM read: count=%lu samples=%u max_abs=%d",
                 (unsigned long)debug_statistics_.input_count, data.size(), max_abs);
    }

#if CONFIG_USE_AUDIO_DEBUGGER
    // 音频调试：发送原始音频数据
    if (audio_debugger_ == nullptr) {
        audio_debugger_ = std::make_unique<AudioDebugger>();
    }
    audio_debugger_->Feed(data);
#endif

    return true;
}

void AudioService::AudioInputTask() {
    while (true) {
        EventBits_t bits = xEventGroupWaitBits(event_group_, AS_EVENT_AUDIO_TESTING_RUNNING |
            AS_EVENT_WAKE_WORD_RUNNING | AS_EVENT_AUDIO_PROCESSOR_RUNNING,
            pdFALSE, pdFALSE, portMAX_DELAY);

        if (service_stopped_) {
            break;
        }
        if (audio_input_need_warmup_) {
            audio_input_need_warmup_ = false;
            vTaskDelay(pdMS_TO_TICKS(120));
            continue;
        }

        /* Used for audio testing in NetworkConfiguring mode by clicking the BOOT button */
        if (bits & AS_EVENT_AUDIO_TESTING_RUNNING) {
            if (audio_testing_queue_.size() >= AUDIO_TESTING_MAX_DURATION_MS / OPUS_FRAME_DURATION_MS) {
                ESP_LOGW(TAG, "Audio testing queue is full, stopping audio testing");
                EnableAudioTesting(false);
                continue;
            }
            std::vector<int16_t> data;
            int samples = OPUS_FRAME_DURATION_MS * 16000 / 1000;
            if (ReadAudioData(data, 16000, samples)) {
                // If input channels is 2, we need to fetch the left channel data
                if (codec_->input_channels() == 2) {
                    auto mono_data = std::vector<int16_t>(data.size() / 2);
                    for (size_t i = 0, j = 0; i < mono_data.size(); ++i, j += 2) {
                        mono_data[i] = data[j];
                    }
                    data = std::move(mono_data);
                }
                PushTaskToEncodeQueue(kAudioTaskTypeEncodeToTestingQueue, std::move(data));
                continue;
            }
        }

        /* Feed the wake word and/or audio processor */
        if (bits & (AS_EVENT_WAKE_WORD_RUNNING | AS_EVENT_AUDIO_PROCESSOR_RUNNING)) {
            int samples = 160; // 10ms
            std::vector<int16_t> data;
            if (ReadAudioData(data, 16000, samples)) {
                if (bits & AS_EVENT_WAKE_WORD_RUNNING) {
                    wake_word_->Feed(data);
                }
                if (bits & AS_EVENT_AUDIO_PROCESSOR_RUNNING) {
                    audio_processor_->Feed(std::move(data));
                }
                continue;
            }
        }

        // Read timeout/error should not terminate the input task.
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    ESP_LOGW(TAG, "Audio input task stopped");
}

void AudioService::AudioOutputTask() {
    while (true) {
        std::unique_lock<std::mutex> lock(audio_queue_mutex_);
        audio_queue_cv_.wait(lock, [this]() { return !audio_playback_queue_.empty() || service_stopped_; });
        if (service_stopped_) {
            break;
        }

        auto task = std::move(audio_playback_queue_.front());
        audio_playback_queue_.pop_front();
        audio_queue_cv_.notify_all();
        lock.unlock();

        if (!codec_->output_enabled()) {
            esp_timer_stop(audio_power_timer_);
            esp_timer_start_periodic(audio_power_timer_, AUDIO_POWER_CHECK_INTERVAL_MS * 1000);
            codec_->EnableOutput(true);
        }

        codec_->OutputData(task->pcm);

        /* Update the last output time */
        last_output_time_ = std::chrono::steady_clock::now();
        debug_statistics_.playback_count++;

#if CONFIG_USE_SERVER_AEC
        /* Record the timestamp for server AEC */
        if (task->timestamp > 0) {
            lock.lock();
            timestamp_queue_.push_back(task->timestamp);
        }
#endif
    }

    ESP_LOGW(TAG, "Audio output task stopped");
}

void AudioService::OpusCodecTask() {
    while (true) {
        std::unique_lock<std::mutex> lock(audio_queue_mutex_);
        audio_queue_cv_.wait(lock, [this]() {
            return service_stopped_ ||
                (!audio_encode_queue_.empty() && audio_send_queue_.size() < MAX_SEND_PACKETS_IN_QUEUE) ||
                (!audio_decode_queue_.empty() && audio_playback_queue_.size() < MAX_PLAYBACK_TASKS_IN_QUEUE);
        });
        if (service_stopped_) {
            break;
        }

        /* Decode the audio from decode queue */
        if (!audio_decode_queue_.empty() && audio_playback_queue_.size() < MAX_PLAYBACK_TASKS_IN_QUEUE) {
            auto packet = std::move(audio_decode_queue_.front());
            audio_decode_queue_.pop_front();
            audio_queue_cv_.notify_all();
            lock.unlock();

            try {
                auto task = std::make_unique<AudioTask>();
                task->type = kAudioTaskTypeDecodeToPlaybackQueue;
                task->timestamp = packet->timestamp;

                if (!SetDecodeSampleRate(packet->sample_rate, packet->frame_duration)) {
                    debug_statistics_.decode_count++;
                    lock.lock();
                    continue;
                }
                if (opus_decoder_ != nullptr) {
                    task->pcm.resize(decoder_frame_size_);
                    esp_audio_dec_in_raw_t raw = {
                        .buffer = (uint8_t *)(packet->payload.data()),
                        .len = (uint32_t)(packet->payload.size()),
                        .consumed = 0,
                        .frame_recover = ESP_AUDIO_DEC_RECOVERY_NONE,
                    };
                    esp_audio_dec_out_frame_t out_frame = {
                        .buffer = (uint8_t *)(task->pcm.data()),
                        .len = (uint32_t)(task->pcm.size() * sizeof(int16_t)),
                        .decoded_size = 0,
                    };
                    esp_audio_dec_info_t dec_info = {};
                    std::unique_lock<std::mutex> decoder_lock(decoder_mutex_);
                    auto ret = esp_opus_dec_decode(opus_decoder_, &raw, &out_frame, &dec_info);
                    decoder_lock.unlock();
                    if (ret == ESP_AUDIO_ERR_OK) {
                        task->pcm.resize(out_frame.decoded_size / sizeof(int16_t));
                        if (decoder_sample_rate_ != codec_->output_sample_rate() && output_resampler_ != nullptr) {
                            uint32_t target_size = 0;
                            esp_ae_rate_cvt_get_max_out_sample_num(output_resampler_, task->pcm.size(), &target_size);
                            std::vector<int16_t> resampled(target_size);
                            uint32_t actual_output = target_size;
                            esp_ae_rate_cvt_process(output_resampler_, (esp_ae_sample_t)task->pcm.data(), task->pcm.size(),
                                                    (esp_ae_sample_t)resampled.data(), &actual_output);
                            resampled.resize(actual_output);
                            task->pcm = std::move(resampled);
                        }
                        lock.lock();
                        audio_playback_queue_.push_back(std::move(task));
                        audio_queue_cv_.notify_all();
                        debug_statistics_.decode_count++;
                        if ((debug_statistics_.decode_count % kOpusCodecStackLogInterval) == 0) {
                            ESP_LOGI(TAG, "Opus codec task: decode_count=%lu stack_high_water=%u free_heap=%u largest_8bit=%u",
                                     (unsigned long)debug_statistics_.decode_count,
                                     static_cast<unsigned>(uxTaskGetStackHighWaterMark(nullptr)),
                                     static_cast<unsigned>(esp_get_free_heap_size()),
                                     static_cast<unsigned>(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)));
                        }
                    } else {
                        ESP_LOGE(TAG, "Failed to decode audio after resize, error code: %d", ret);
                        lock.lock();
                    }
                } else {
                    ESP_LOGE(TAG, "Audio decoder is not configured");
                    lock.lock();
                }
            } catch (const std::bad_alloc&) {
                ESP_LOGE(TAG,
                         "Drop decoded audio packet due to bad_alloc: payload=%u sample_rate=%d frame_duration=%d free_heap=%u largest_8bit=%u",
                         static_cast<unsigned>(packet->payload.size()),
                         packet->sample_rate,
                         packet->frame_duration,
                         static_cast<unsigned>(esp_get_free_heap_size()),
                         static_cast<unsigned>(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)));
                lock.lock();
            } catch (...) {
                ESP_LOGE(TAG, "Drop decoded audio packet due to unexpected exception");
                lock.lock();
            }
            debug_statistics_.decode_count++;
        }
        /* Encode the audio to send queue */
        if (!audio_encode_queue_.empty() && audio_send_queue_.size() < MAX_SEND_PACKETS_IN_QUEUE) {
            auto task = std::move(audio_encode_queue_.front());
            audio_encode_queue_.pop_front();
            audio_queue_cv_.notify_all();
            lock.unlock();

            auto packet = std::make_unique<AudioStreamPacket>();
            packet->frame_duration = OPUS_FRAME_DURATION_MS;
            packet->sample_rate = 16000;
            packet->timestamp = task->timestamp;

            if (opus_encoder_ != nullptr && task->pcm.size() == encoder_frame_size_) {
                std::vector<uint8_t> buf(encoder_outbuf_size_);
                esp_audio_enc_in_frame_t in = {
                    .buffer = (uint8_t *)(task->pcm.data()),
                    .len = (uint32_t)(encoder_frame_size_ * sizeof(int16_t)),
                };
                esp_audio_enc_out_frame_t out = {
                    .buffer = buf.data(),
                    .len = (uint32_t)encoder_outbuf_size_,
                    .encoded_bytes = 0,
                };
                auto ret = esp_opus_enc_process(opus_encoder_, &in, &out);
                if (ret == ESP_AUDIO_ERR_OK) {
                    packet->payload.assign(buf.data(), buf.data() + out.encoded_bytes);

                    if (task->type == kAudioTaskTypeEncodeToSendQueue) {
                        {
                            std::lock_guard<std::mutex> lock2(audio_queue_mutex_);
                            audio_send_queue_.push_back(std::move(packet));
                        }
                        if ((debug_statistics_.encode_count % 20) == 0) {
                            ESP_LOGI(TAG, "Opus encoded for send: count=%lu queue=%u",
                                     (unsigned long)debug_statistics_.encode_count, audio_send_queue_.size());
                        }
                        if (callbacks_.on_send_queue_available) {
                            callbacks_.on_send_queue_available();
                        }
                    } else if (task->type == kAudioTaskTypeEncodeToTestingQueue) {
                        std::lock_guard<std::mutex> lock2(audio_queue_mutex_);
                        audio_testing_queue_.push_back(std::move(packet));
                    }
                    debug_statistics_.encode_count++;
                    if ((debug_statistics_.encode_count % kOpusCodecStackLogInterval) == 0) {
                        ESP_LOGI(TAG, "Opus codec task: encode_count=%lu stack_high_water=%u free_heap=%u largest_8bit=%u",
                                 (unsigned long)debug_statistics_.encode_count,
                                 static_cast<unsigned>(uxTaskGetStackHighWaterMark(nullptr)),
                                 static_cast<unsigned>(esp_get_free_heap_size()),
                                 static_cast<unsigned>(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)));
                    }
                } else {
                    ESP_LOGE(TAG, "Failed to encode audio, error code: %d", ret);
                }
            } else {
                ESP_LOGE(TAG, "Failed to encode audio: encoder not configured or invalid frame size (got %u, expected %u)",
                         task->pcm.size(), encoder_frame_size_);
            }
            lock.lock();
        }
    }

    ESP_LOGW(TAG, "Opus codec task stopped");
}

bool AudioService::SetDecodeSampleRate(int sample_rate, int frame_duration) {
    {
        std::unique_lock<std::mutex> decoder_lock(decoder_mutex_);
        if (opus_decoder_ != nullptr && decoder_sample_rate_ == sample_rate && decoder_duration_ms_ == frame_duration) {
            return true;
        }
    }

    auto codec = Board::GetInstance().GetAudioCodec();

    void* new_decoder = nullptr;
    esp_opus_dec_cfg_t opus_dec_cfg = OPUS_DEC_CFG(sample_rate, frame_duration);
    auto ret = esp_opus_dec_open(&opus_dec_cfg, sizeof(esp_opus_dec_cfg_t), &new_decoder);
    if (new_decoder == nullptr) {
        ESP_LOGE(TAG,
                 "Failed to create audio decoder: sample_rate=%d frame_duration=%d error=%d free_heap=%u largest_8bit=%u",
                 sample_rate, frame_duration, ret,
                 static_cast<unsigned>(esp_get_free_heap_size()),
                 static_cast<unsigned>(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)));
        return false;
    }

    esp_ae_rate_cvt_handle_t new_output_resampler = nullptr;
    if (sample_rate != codec->output_sample_rate()) {
        ESP_LOGI(TAG, "Resampling audio from %d to %d", sample_rate, codec->output_sample_rate());
        esp_ae_rate_cvt_cfg_t output_resampler_cfg = RATE_CVT_CFG(
            sample_rate, codec->output_sample_rate(), ESP_AUDIO_MONO);
        auto resampler_ret = esp_ae_rate_cvt_open(&output_resampler_cfg, &new_output_resampler);
        if (new_output_resampler == nullptr) {
            esp_opus_dec_close(new_decoder);
            ESP_LOGE(TAG,
                     "Failed to create output resampler: input=%d output=%d error=%d free_heap=%u largest_8bit=%u",
                     sample_rate, codec->output_sample_rate(), resampler_ret,
                     static_cast<unsigned>(esp_get_free_heap_size()),
                     static_cast<unsigned>(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)));
            return false;
        }
    }

    void* old_decoder = nullptr;
    esp_ae_rate_cvt_handle_t old_output_resampler = nullptr;
    {
        std::unique_lock<std::mutex> decoder_lock(decoder_mutex_);
        old_decoder = opus_decoder_;
        old_output_resampler = output_resampler_;
        opus_decoder_ = new_decoder;
        output_resampler_ = new_output_resampler;
        decoder_sample_rate_ = sample_rate;
        decoder_duration_ms_ = frame_duration;
        decoder_frame_size_ = decoder_sample_rate_ / 1000 * frame_duration;
    }

    if (old_decoder != nullptr) {
        esp_opus_dec_close(old_decoder);
    }
    if (old_output_resampler != nullptr) {
        esp_ae_rate_cvt_close(old_output_resampler);
    }

    ESP_LOGI(TAG, "Opus decoder configured: %d Hz, %d ms, frame_samples=%d",
             decoder_sample_rate_, decoder_duration_ms_, decoder_frame_size_);
    return true;
}

void AudioService::PushTaskToEncodeQueue(AudioTaskType type, std::vector<int16_t>&& pcm) {
    auto task = std::make_unique<AudioTask>();
    task->type = type;
    task->pcm = std::move(pcm);
    /* Push the task to the encode queue */
    std::unique_lock<std::mutex> lock(audio_queue_mutex_);

    /* If the task is to send queue, we need to set the timestamp */
    if (type == kAudioTaskTypeEncodeToSendQueue && !timestamp_queue_.empty()) {
        if (timestamp_queue_.size() <= MAX_TIMESTAMPS_IN_QUEUE) {
            task->timestamp = timestamp_queue_.front();
        } else {
            ESP_LOGW(TAG, "Timestamp queue (%u) is full, dropping timestamp", timestamp_queue_.size());
        }
        timestamp_queue_.pop_front();
    }

    audio_queue_cv_.wait(lock, [this]() { return audio_encode_queue_.size() < MAX_ENCODE_TASKS_IN_QUEUE; });
    audio_encode_queue_.push_back(std::move(task));
    audio_queue_cv_.notify_all();
}

bool AudioService::PushPacketToDecodeQueue(std::unique_ptr<AudioStreamPacket> packet, bool wait,
                                           const std::function<bool()>& cancel_wait) {
    std::unique_lock<std::mutex> lock(audio_queue_mutex_);
    if (audio_decode_queue_.size() >= MAX_DECODE_PACKETS_IN_QUEUE) {
        if (wait) {
            audio_queue_cv_.wait(lock, [this, &cancel_wait]() {
                return audio_decode_queue_.size() < MAX_DECODE_PACKETS_IN_QUEUE ||
                    (cancel_wait && cancel_wait()) || service_stopped_;
            });
            if (service_stopped_ || (cancel_wait && cancel_wait())) {
                return false;
            }
        } else {
            return false;
        }
    }
    audio_decode_queue_.push_back(std::move(packet));
    audio_queue_cv_.notify_all();
    return true;
}

std::unique_ptr<AudioStreamPacket> AudioService::PopPacketFromSendQueue() {
    std::lock_guard<std::mutex> lock(audio_queue_mutex_);
    if (audio_send_queue_.empty()) {
        return nullptr;
    }
    auto packet = std::move(audio_send_queue_.front());
    audio_send_queue_.pop_front();
    audio_queue_cv_.notify_all();
    return packet;
}

void AudioService::EncodeWakeWord() {
    if (wake_word_) {
        wake_word_->EncodeWakeWordData();
    }
}

const std::string& AudioService::GetLastWakeWord() const {
    return wake_word_->GetLastDetectedWakeWord();
}

std::unique_ptr<AudioStreamPacket> AudioService::PopWakeWordPacket() {
    auto packet = std::make_unique<AudioStreamPacket>();
    if (wake_word_->GetWakeWordOpus(packet->payload)) {
        return packet;
    }
    return nullptr;
}

void AudioService::EnableWakeWordDetection(bool enable) {
    if (!wake_word_) {
        return;
    }

    ESP_LOGD(TAG, "%s wake word detection", enable ? "Enabling" : "Disabling");
    if (enable) {
        if (!wake_word_initialized_) {
#if CONFIG_IDF_TARGET_ESP32C5
            const size_t free_heap = esp_get_free_heap_size();
            const size_t largest_8bit = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
            if (free_heap < kMinHeapForWakeWordInit || largest_8bit < kMinLargestBlockForWakeWordInit) {
                ESP_LOGW(TAG,
                         "Skip wake word initialization due to low memory: free_heap=%u largest_8bit=%u",
                         static_cast<unsigned>(free_heap),
                         static_cast<unsigned>(largest_8bit));
                return;
            }
#endif
            if (!wake_word_->Initialize(codec_, models_list_)) {
                ESP_LOGE(TAG, "Failed to initialize wake word");
                return;
            }
            wake_word_initialized_ = true;
        }
        // Reset input resampler to clear cached data from previous mode (e.g. AudioProcessor)
        // This prevents buffer overflow when switching between different feed sizes
        {
            std::lock_guard<std::mutex> lock(input_resampler_mutex_);
            if (input_resampler_ != nullptr) {
                esp_ae_rate_cvt_reset(input_resampler_);
            }
        }
        wake_word_->Start();
        xEventGroupSetBits(event_group_, AS_EVENT_WAKE_WORD_RUNNING);
    } else {
        wake_word_->Stop();
        xEventGroupClearBits(event_group_, AS_EVENT_WAKE_WORD_RUNNING);
    }
}

void AudioService::EnableVoiceProcessing(bool enable) {
    ESP_LOGI(TAG, "%s voice processing", enable ? "Enabling" : "Disabling");
    if (enable) {
        if (!audio_processor_initialized_) {
            audio_processor_->Initialize(codec_, OPUS_FRAME_DURATION_MS, models_list_);
            audio_processor_initialized_ = true;
        }

        /* We should make sure no audio is playing */
        ResetDecoder();
        audio_input_need_warmup_ = true;
        // Reset input resampler to clear cached data from previous mode (e.g. WakeWord)
        // This prevents buffer overflow when switching between different feed sizes
        {
            std::lock_guard<std::mutex> lock(input_resampler_mutex_);
            if (input_resampler_ != nullptr) {
                esp_ae_rate_cvt_reset(input_resampler_);
            }
        }
        audio_processor_->Start();
        xEventGroupSetBits(event_group_, AS_EVENT_AUDIO_PROCESSOR_RUNNING);
    } else {
        audio_processor_->Stop();
        xEventGroupClearBits(event_group_, AS_EVENT_AUDIO_PROCESSOR_RUNNING);
    }
}

void AudioService::EnableAudioTesting(bool enable) {
    ESP_LOGI(TAG, "%s audio testing", enable ? "Enabling" : "Disabling");
    if (enable) {
        xEventGroupSetBits(event_group_, AS_EVENT_AUDIO_TESTING_RUNNING);
    } else {
        xEventGroupClearBits(event_group_, AS_EVENT_AUDIO_TESTING_RUNNING);
        /* Copy audio_testing_queue_ to audio_decode_queue_ */
        std::lock_guard<std::mutex> lock(audio_queue_mutex_);
        audio_decode_queue_ = std::move(audio_testing_queue_);
        audio_queue_cv_.notify_all();
    }
}

void AudioService::EnableDeviceAec(bool enable) {
    ESP_LOGI(TAG, "%s device AEC", enable ? "Enabling" : "Disabling");
    if (!audio_processor_initialized_) {
        audio_processor_->Initialize(codec_, OPUS_FRAME_DURATION_MS, models_list_);
        audio_processor_initialized_ = true;
    }

    audio_processor_->EnableDeviceAec(enable);
}

void AudioService::SetCallbacks(AudioServiceCallbacks& callbacks) {
    callbacks_ = callbacks;
}

void AudioService::PlaySound(const std::string_view& ogg) {
    PlaySoundInternal(ogg, false, {});
}

void AudioService::PlaySoundBlocking(const std::string_view& ogg) {
    PlaySoundInternal(ogg, true, {});
}

void AudioService::PlaySoundBlocking(const std::string_view& ogg, const std::function<bool()>& cancel_playback) {
    PlaySoundInternal(ogg, true, cancel_playback);
}

bool AudioService::ShouldCancelSoundPlayback() {
    std::lock_guard<std::mutex> lock(sound_playback_state_mutex_);
    return sound_cancel_playback_ && sound_cancel_playback_();
}

void AudioService::PlaySoundInternal(const std::string_view& ogg, bool wait_for_queue,
                                     const std::function<bool()>& cancel_playback) {
    const size_t free_heap = esp_get_free_heap_size();
    const size_t largest_8bit = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    if (free_heap < kMinHeapForSoundPlayback || largest_8bit < kMinLargestBlockForSoundPlayback) {
        ESP_LOGW(TAG, "Skip sound playback due to low memory: free_heap=%u largest_8bit=%u",
                 static_cast<unsigned>(free_heap),
                 static_cast<unsigned>(largest_8bit));
        return;
    }

    if (!codec_->output_enabled()) {
        esp_timer_stop(audio_power_timer_);
        esp_timer_start_periodic(audio_power_timer_, AUDIO_POWER_CHECK_INTERVAL_MS * 1000);
        codec_->EnableOutput(true);
    }

    const auto* buf = reinterpret_cast<const uint8_t*>(ogg.data());
    size_t size = ogg.size();

    std::lock_guard<std::mutex> demuxer_lock(sound_demuxer_mutex_);
    {
        std::lock_guard<std::mutex> playback_lock(sound_playback_state_mutex_);
        sound_wait_for_queue_ = wait_for_queue;
        sound_cancel_playback_ = cancel_playback;
    }
    if (sound_demuxer_ == nullptr) {
        auto* demuxer = new (std::nothrow) OggDemuxer();
        if (demuxer == nullptr) {
            ESP_LOGE(TAG, "Failed to allocate sound demuxer");
            return;
        }
        demuxer->OnDemuxerFinished([this](const uint8_t* data, int sample_rate, size_t size) {
            if (ShouldCancelSoundPlayback()) {
                return;
            }

            const size_t packet_free_heap = esp_get_free_heap_size();
            const size_t packet_largest_8bit = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
            if (packet_free_heap < kMinHeapForSoundQueuePacket ||
                packet_largest_8bit < kMinLargestBlockForSoundQueuePacket) {
                ESP_LOGW(TAG,
                         "Drop sound packet due to low memory: free_heap=%u largest_8bit=%u sample_rate=%d size=%u",
                         static_cast<unsigned>(packet_free_heap),
                         static_cast<unsigned>(packet_largest_8bit),
                         sample_rate,
                         static_cast<unsigned>(size));
                return;
            }

            try {
                auto packet = std::make_unique<AudioStreamPacket>();
                packet->sample_rate = sample_rate;
                packet->frame_duration = 60;
                packet->payload.resize(size);
                std::memcpy(packet->payload.data(), data, size);
                bool wait = false;
                std::function<bool()> cancel_wait;
                {
                    std::lock_guard<std::mutex> playback_lock(sound_playback_state_mutex_);
                    wait = sound_wait_for_queue_;
                    cancel_wait = sound_cancel_playback_;
                }
                if (!PushPacketToDecodeQueue(std::move(packet), wait, cancel_wait)) {
                    ESP_LOGW(TAG, "Drop sound packet because decode queue is full");
                }
            } catch (const std::bad_alloc&) {
                ESP_LOGE(TAG, "Drop sound packet due to bad_alloc");
            } catch (...) {
                ESP_LOGE(TAG, "Drop sound packet due to unexpected exception");
            }
        });
        sound_demuxer_.reset(demuxer);
    }

    try {
        if (ShouldCancelSoundPlayback()) {
            return;
        }
        sound_demuxer_->Reset();
        sound_demuxer_->Process(buf, size);
    } catch (const std::bad_alloc&) {
        ESP_LOGE(TAG, "Skip sound playback due to bad_alloc");
    } catch (...) {
        ESP_LOGE(TAG, "Skip sound playback due to unexpected exception");
    }
}

bool AudioService::IsIdle() {
    std::lock_guard<std::mutex> lock(audio_queue_mutex_);
    return audio_encode_queue_.empty() && audio_decode_queue_.empty() && audio_playback_queue_.empty() && audio_testing_queue_.empty();
}

void AudioService::WaitForPlaybackQueueEmpty() {
    std::unique_lock<std::mutex> lock(audio_queue_mutex_);
    audio_queue_cv_.wait(lock, [this]() { 
        return service_stopped_ || (audio_decode_queue_.empty() && audio_playback_queue_.empty()); 
    });
}

void AudioService::ResetDecoder() {
    std::lock_guard<std::mutex> lock(audio_queue_mutex_);
    std::unique_lock<std::mutex> decoder_lock(decoder_mutex_);
    if (opus_decoder_ != nullptr) {
        esp_opus_dec_reset(opus_decoder_);
    }
    decoder_lock.unlock();
    timestamp_queue_.clear();
    audio_decode_queue_.clear();
    audio_playback_queue_.clear();
    audio_testing_queue_.clear();
    audio_queue_cv_.notify_all();
}

void AudioService::CheckAndUpdateAudioPowerState() {
    auto now = std::chrono::steady_clock::now();
    auto input_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_input_time_).count();
    auto output_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_output_time_).count();
    if (input_elapsed > AUDIO_POWER_TIMEOUT_MS && codec_->input_enabled()) {
        codec_->EnableInput(false);
    }
    if (output_elapsed > AUDIO_POWER_TIMEOUT_MS && codec_->output_enabled()) {
        // Keep TX clock when duplex RX is active; otherwise RX may stall on some boards.
        if (!(codec_->duplex() && codec_->input_enabled())) {
            codec_->EnableOutput(false);
        }
    }
    if (!codec_->input_enabled() && !codec_->output_enabled()) {
        esp_timer_stop(audio_power_timer_);
    }
}

void AudioService::SetModelsList(srmodel_list_t* models_list) {
    models_list_ = models_list;

#if CONFIG_WAKE_WORD_DISABLED
    wake_word_ = nullptr;
#else
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32P4
    if (esp_srmodel_filter(models_list_, ESP_MN_PREFIX, NULL) != nullptr) {
        wake_word_ = std::make_unique<CustomWakeWord>();
    } else if (esp_srmodel_filter(models_list_, ESP_WN_PREFIX, NULL) != nullptr) {
        wake_word_ = std::make_unique<AfeWakeWord>();
    } else {
        wake_word_ = nullptr;
    }
#else
    if (esp_srmodel_filter(models_list_, ESP_WN_PREFIX, NULL) != nullptr) {
        wake_word_ = std::make_unique<EspWakeWord>();
    } else {
        wake_word_ = nullptr;
    }
#endif
#endif

    if (wake_word_) {
        wake_word_->OnWakeWordDetected([this](const std::string& wake_word) {
            if (callbacks_.on_wake_word_detected) {
                callbacks_.on_wake_word_detected(wake_word);
            }
        });
    }
}

bool AudioService::IsAfeWakeWord() {
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32P4
    return wake_word_ != nullptr && dynamic_cast<AfeWakeWord*>(wake_word_.get()) != nullptr;
#else
    return false;
#endif
}
