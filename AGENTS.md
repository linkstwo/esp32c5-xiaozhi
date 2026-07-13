# Codex project instructions

These rules apply to every task in this repository. The priority is to finish the requested change correctly while minimizing token usage, tool calls, build time, and unnecessary file changes.

## Token and time discipline

- Inspect only the files directly related to the task first. Do not scan the whole repository unless the local evidence is insufficient.
- Use targeted searches and partial file reads. Avoid repeatedly reopening the same large files or printing long generated/image source files.
- Form a concrete hypothesis before running commands. Do not use repeated trial-and-error commands without narrowing the cause.
- Keep progress messages and final reports concise. Report decisions, changed files, verification performed, and remaining risks; do not narrate every command.
- Do not create plans, notes, temporary reports, or duplicate documentation unless they are required for the implementation.
- Reuse existing project patterns, fonts, assets, components, and helper functions before adding anything new.
- Make the smallest coherent patch that completes the requested behavior. Do not perform unrelated refactors or cosmetic cleanup.

## Build and simulator discipline

- Do not start with a full ESP-IDF or LVGL simulator build. First inspect the relevant source, make the targeted edit, and run the cheapest meaningful validation.
- Prefer syntax checks, existing incremental builds, or building only the affected target. Run a full clean build only when it is necessary to prove correctness or when the user explicitly requests it.
- Before any potentially long-running build, check whether the environment and paths are compatible. On Windows/MinGW, avoid creating simulator workspaces under paths containing Chinese characters or other path-sensitive characters.
- Use a reasonable timeout for experimental builds. If a build is clearly blocked by environment setup, stop early, explain the exact blocker, and do not repeatedly rebuild the same configuration.
- Do not install or introduce SDL2, fonts, image libraries, or other dependencies merely to obtain a screenshot unless the task explicitly requires them and the expected benefit justifies the cost.
- Never leave temporary simulator files, generated build directories, copied repositories, or experimental configuration changes in the project after an unsuccessful attempt.

## ESP32 resource constraints

- Treat flash, application partition space, RAM, stack, and LVGL animation load as hard constraints.
- Do not add a new Chinese font just to change one or two labels. Reuse the existing Chinese font or adjust layout/zoom/spacing.
- Do not add new image assets when an existing asset or LVGL primitive can achieve the result.
- After adding fonts, images, or large generated C arrays, check binary/partition impact. Avoid changes that leave an unsafe application-partition margin.
- Prefer integer math and lightweight LVGL timers/animations over floating-point work in frequently executed UI callbacks.
- Ensure timers and animations are created once, paused/resumed correctly, and deleted when their owning screen is destroyed. Avoid duplicate callbacks and dangling object references.

## UI implementation rules

- Do not substitute a static decoration for requested dynamic behavior. If a waveform, progress indicator, spectrum, or animation is specified as moving, implement the actual runtime update path.
- Verify state restoration when leaving and re-entering a screen. A playing, paused, running, or finished state must render correctly after navigation.
- UI labels must match real behavior. Do not show controls such as “休息”, configurable duration, track index, or volume unless the corresponding behavior/data is actually implemented.
- Do not display invented or fixed placeholder telemetry as if it were live data. Clearly remove it or connect it to the real source.
- Preserve swipe/navigation behavior and touch event propagation when changing widgets.

## Verification and honesty

- Never claim that a simulator, firmware build, screenshot, animation, or hardware behavior was verified unless it was actually run or observed.
- Do not use a mock or unrelated image as proof of the final UI.
- When simulation is unavailable, perform static verification and state explicitly what remains unverified.
- Before finishing, review the diff for accidental generated files, new fonts/assets, duplicate timers, hard-coded fake values, and unrelated edits.

## Current project-specific lesson

The music page waveform and spectrum bars are separate elements. Animating only the bars does not satisfy a request for a moving waveform. For music-page work, inspect both `ui_Music_WaveLine`/`ui_Music_WaveGlow` and the bar animations, including pause, unload, reload, and resume behavior.
