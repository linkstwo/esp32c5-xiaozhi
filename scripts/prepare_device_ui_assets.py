#!/usr/bin/env python3
"""Convert the selected device UI pack PNGs into LVGL C image descriptors."""

import argparse
from pathlib import Path

from PIL import Image


ASSETS = {
    "device_v3_bg": "backgrounds/device_bg_284x240.png",
    "device_v3_detail_bg": "backgrounds/device_detail_bg_284x240.png",
    "device_v3_card_wave": "decoration/card_wave_right.png",
    "device_v3_corner_flower": "decoration/corner_flower.png",
    "device_v3_header_left": "decoration/header_flourish_left.png",
    "device_v3_header_right": "decoration/header_flourish_right.png",
    "device_v3_bottom_banner": "status/bottom_banner.png",
    "device_v3_tab_active": "status/tab_active.png",
    "device_v3_pill_ok": "status/pill_ok.png",
    "device_v3_pill_warning": "status/pill_warning.png",
    "device_v3_pill_offline": "status/pill_offline.png",
    "device_v3_icon_shield": "icons/32/shield_ok.png",
    "device_v3_icon_cloud": "icons/32/cloud.png",
    "device_v3_icon_environment": "icons/32/environment.png",
    "device_v3_icon_speaker": "icons/32/speaker.png",
    "device_v3_icon_back": "icons/24/back.png",
    "device_v3_icon_arrow": "icons/24/arrow_right.png",
    "device_v3_icon_chip": "icons/24/chip.png",
    "device_v3_icon_board": "icons/24/board.png",
    "device_v3_icon_firmware": "icons/24/firmware.png",
    "device_v3_icon_clock": "icons/24/clock.png",
    "device_v3_icon_memory": "icons/24/memory.png",
    "device_v3_icon_ota": "icons/24/ota.png",
    "device_v3_icon_wifi": "icons/24/wifi.png",
    "device_v3_icon_router": "icons/24/router.png",
    "device_v3_icon_signal": "icons/24/signal.png",
    "device_v3_icon_ip": "icons/24/ip.png",
    "device_v3_icon_mqtt": "icons/24/mqtt.png",
    "device_v3_icon_udp": "icons/24/udp_audio.png",
    "device_v3_icon_bme": "icons/24/bme690.png",
    "device_v3_icon_temperature": "icons/24/thermometer.png",
    "device_v3_icon_humidity": "icons/24/humidity.png",
    "device_v3_icon_pressure": "icons/24/pressure.png",
    "device_v3_icon_iaq": "icons/24/iaq.png",
    "device_v3_icon_co2": "icons/24/co2.png",
    "device_v3_icon_accuracy": "icons/24/accuracy.png",
    "device_v3_icon_microphone": "icons/24/microphone.png",
    "device_v3_icon_decoder": "icons/24/decoder.png",
    "device_v3_icon_volume": "icons/24/volume.png",
}


def rgb565(red: int, green: int, blue: int) -> tuple[int, int]:
    value = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3)
    return value & 0xFF, value >> 8


def c_bytes(data: bytes) -> str:
    rows = []
    for start in range(0, len(data), 16):
        rows.append("    " + ",".join(f"0x{byte:02X}" for byte in data[start:start + 16]) + ",")
    return "\n".join(rows)


def convert(name: str, source: Path) -> str:
    image = Image.open(source).convert("RGBA")
    rgba = list(image.getdata())
    has_alpha = any(alpha != 255 for _, _, _, alpha in rgba)
    color = bytearray()
    alpha = bytearray()
    for red, green, blue, opacity in rgba:
        low, high = rgb565(red, green, blue)
        color.extend((low, high))
        if has_alpha:
            alpha.append(opacity)
    data = bytes(color + alpha)
    color_format = "RGB565A8" if has_alpha else "RGB565"
    return f'''static const LV_ATTRIBUTE_MEM_ALIGN uint8_t ui_img_{name}_data[] = {{
{c_bytes(data)}
}};

const lv_image_dsc_t ui_img_{name} = {{
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.cf = LV_COLOR_FORMAT_{color_format},
    .header.flags = 0,
    .header.w = {image.width},
    .header.h = {image.height},
    .header.stride = {image.width * 2},
    .data_size = sizeof(ui_img_{name}_data),
    .data = ui_img_{name}_data,
}};

'''


def main() -> None:
    parser = argparse.ArgumentParser(description="Prepare device page LVGL image assets")
    parser.add_argument("--source", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()

    source = args.source.resolve()
    output = args.output.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    result = [
        "// Generated from the device page UI resource pack.\n",
        "#include \"../ui.h\"\n\n",
        "#ifndef LV_ATTRIBUTE_MEM_ALIGN\n#define LV_ATTRIBUTE_MEM_ALIGN\n#endif\n\n",
    ]
    for name, relative_path in ASSETS.items():
        result.append(convert(name, source / "assets" / relative_path))
    output.write_text("".join(result), encoding="utf-8", newline="\n")
    print(f"Prepared {len(ASSETS)} device UI assets in {output}")


if __name__ == "__main__":
    main()
