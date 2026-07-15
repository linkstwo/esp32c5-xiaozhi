# Codex task: forest-spirit focus timer UI

## Objective

Finish the focus-timer page so the firmware matches the approved forest-spirit mockup as closely as possible **inside the real 284 x 240 LCD canvas**. The desktop simulator title bar is outside the product screen and must never be counted in layout coordinates.

The base visual implementation is already on branch `ui/focus-timer-forest` in:

- `main/display/smart_gadget_ui/screens/ui_Today.c`
- `main/display/smart_gadget_ui/screens/focus_ui_tokens.h`

Do not redesign the page into a different style. Refine and verify this implementation.

## Hard constraints

1. LCD content area is exactly `284 x 240`.
2. Keep the page light: no full-screen background bitmap, no blur shader, no GIF, no large new animation asset.
3. Reuse the four existing mascot assets and the existing LVGL native-alpha format.
4. Keep touch targets at least 40 px high.
5. Keep the main timer visually dominant and the bottom center button dominant.
6. Do not restore pagination dots on this page; swipe navigation already works.
7. Do not modify unrelated music, call, weather, audio, network, partition, or board code.
8. Build after the UI changes with `idf.py build`.

## Target layout, in LCD pixels

| Element | Position and size |
|---|---|
| Main arc | `x=46, y=5, w=164, h=164` |
| Inner circular panel | `x=58, y=17, w=140, h=140` |
| Mascot | logical anchor `x=195, y=25`, effective zoom about `218/256` |
| Session badge | `x=218, y=8, w=60, h=24` |
| Bottom control shelf | `x=3, y=173, w=278, h=54` |
| Left button | `x=8, y=181, w=66, h=40` |
| Main button | `x=80, y=176, w=124, h=48` |
| Right button | `x=210, y=181, w=66, h=40` |

The mascot may overlap the upper-right edge of the arc, but it must not cover the `25:00` text or the session badge.

## Typography

Use only fonts already compiled into the project:

- Timer digits: `ui_font_Number`, source size 66 px.
- Chinese labels: `font_puhui_16_4`.
- Icons: `BUILTIN_ICON_FONT` / Font Awesome symbols already used by the project.

Recommended effective sizes:

- Timer `25:00`: approximately 42–45 px on screen. In `RenderTodayFocusState()`, use transform zoom around `160–170` instead of the old undersized value when the text is MM:SS.
- Status chip: approximately 13 px effective size.
- Daily summary and badge: approximately 12 px effective size.
- Button Chinese text: 16 px font at native scale; do not shrink it below readable size.

Do not introduce a new large Chinese font unless a missing glyph is confirmed.

## Dynamic text wiring

The visual file currently provides the approved default composition. Wire these labels in `main/display/smart_gadget_display.cc` so they reflect state rather than remaining permanently static:

- `ui_Today_status`
  - Ready: `准备专注`
  - Running: `专注中`
  - Paused: `已暂停`
  - Finished: `已完成`
- `ui_Today_state_label`
  - Display `第 N 番`, where `N = focus_completed_sessions_today_ + 1` before completion and equals the completed count after completion as appropriate.
- `ui_Today_status_detail`
  - Display `今日已专注 N 分钟`.
  - Derive minutes from completed focus sessions or a dedicated accumulated-minute counter. Do not hard-code 50 in the final runtime implementation.

Keep the existing button-state behavior:

- Ready: `专注 / 开始 / 重置`
- Running: `暂停 / 结束 / +5分`
- Paused: `重置 / 继续 / 结束`
- Finished: `休息 / 再来一轮 / 结束`

## Motion and effects

Use restrained effects that are cheap on ESP32-C5:

1. Screen entrance: keep staggered `upanim_Animation()` calls for arc, chip, badge, mascot, and buttons.
2. Arc progress: keep the existing 260 ms interpolation instead of snapping.
3. Mascot state change: keep the existing 220 ms two-layer cross-fade.
4. Button press: keep transform zoom from 256 to about 238.
5. Shadows: use LVGL style shadows only; avoid raster shadow images.
6. Arc marker: the small oval knob must move with the arc automatically. Do not implement a timer-driven floating image unless profiling proves it is safe.
7. Avoid continuous decorative animations. The timer already refreshes every 250 ms; do not add another high-frequency timer.

## Visual hierarchy

- Background: warm cream `#FBF8F1`.
- Primary green: around `#789D66`.
- Main text: dark warm brown around `#5C422F`.
- Orange is reserved for pause/reset emphasis around `#E89A44`.
- Keep the two bottom hill shapes below 20% visual contrast.
- The main button may have a stronger shadow than side buttons, but no glossy plastic bevel.

## Verification checklist

1. Build succeeds with no new warnings from `ui_Today.c`.
2. Open the simulator at the real `284 x 240` content size.
3. Check all four focus states: Ready, Running, Paused, Finished.
4. Confirm no object is clipped at x=284 or y=240.
5. Confirm the mascot never blocks the timer digits.
6. Confirm all three controls respond across their full visible bounds.
7. Confirm the arc knob follows the progress value.
8. Confirm Chinese glyphs render without tofu squares.
9. Capture one screenshot per state and compare spacing, not the simulator window chrome.
10. Re-run `idf.py build` after the final visual adjustment.

## Scope discipline

Make the smallest focused patch possible. Prefer changing only:

- `main/display/smart_gadget_ui/screens/ui_Today.c`
- `main/display/smart_gadget_ui/screens/focus_ui_tokens.h`
- the focus-related methods in `main/display/smart_gadget_display.cc`

Do not regenerate the whole SquareLine project, because that can overwrite unrelated hand-tuned screens.
