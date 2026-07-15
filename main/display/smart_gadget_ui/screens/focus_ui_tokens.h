#pragma once

// Physical LCD canvas used by the product UI. Keep every focus-page element
// inside this 284 x 240 coordinate system; the desktop simulator title bar is
// not part of the drawable area.
#define FOCUS_SCREEN_W 284
#define FOCUS_SCREEN_H 240

// Main focus dial. The dial is deliberately offset a little to the left so the
// mascot can overlap its upper-right edge without covering the time.
#define FOCUS_TIMER_X 46
#define FOCUS_TIMER_Y 5
#define FOCUS_TIMER_SIZE 164
#define FOCUS_TIMER_INNER_X 58
#define FOCUS_TIMER_INNER_Y 17
#define FOCUS_TIMER_INNER_SIZE 140
#define FOCUS_ARC_MAIN_WIDTH 7
#define FOCUS_ARC_INDICATOR_WIDTH 8

// Bottom controls. Side controls retain a 40 px touch height; the primary
// action is 48 px high and visually dominant.
#define FOCUS_LEFT_BTN_X 8
#define FOCUS_LEFT_BTN_Y 181
#define FOCUS_LEFT_BTN_W 66
#define FOCUS_LEFT_BTN_H 40

#define FOCUS_MAIN_BTN_X 80
#define FOCUS_MAIN_BTN_Y 176
#define FOCUS_MAIN_BTN_W 124
#define FOCUS_MAIN_BTN_H 48

#define FOCUS_RIGHT_BTN_X 210
#define FOCUS_RIGHT_BTN_Y 181
#define FOCUS_RIGHT_BTN_W 66
#define FOCUS_RIGHT_BTN_H 40

// One mascot position for every timer state keeps the composition stable and
// avoids the character jumping across the small display.
#define FOCUS_MASCOT_RIGHT_X 195
#define FOCUS_MASCOT_RIGHT_Y 25
#define FOCUS_MASCOT_LEFT_X 195
#define FOCUS_MASCOT_LEFT_Y 25
#define FOCUS_MASCOT_SAFE_W 78
#define FOCUS_MASCOT_SAFE_H 96
#define FOCUS_BUTTON_SAFE_Y_MIN 173

#define FOCUS_PAGE_DOTS_Y 232

// Shared forest palette.
#define FOCUS_COLOR_TEXT_MAIN 0x5C422F
#define FOCUS_COLOR_TEXT_GREEN 0x557347
#define FOCUS_COLOR_TEXT_MUTED 0x967B61
#define FOCUS_COLOR_PANEL 0xFFFDF8
#define FOCUS_COLOR_PANEL_BORDER 0xEADCC3
#define FOCUS_COLOR_TRACK 0xE1DDCB
#define FOCUS_COLOR_DECOR_BLUE 0xB8D4E1
#define FOCUS_COLOR_DECOR_GREEN 0x789D66
#define FOCUS_COLOR_DECOR_YELLOW 0xE9B55D

#define FOCUS_COLOR_READY_BG 0xFBF8F1
#define FOCUS_COLOR_READY_ACCENT 0x789D66
#define FOCUS_COLOR_READY_PRIMARY 0x789D66
#define FOCUS_COLOR_READY_SECONDARY 0xFFFDF8
#define FOCUS_COLOR_READY_LEFT 0xF3F6E9

#define FOCUS_COLOR_RUNNING_BG 0xFBF8F1
#define FOCUS_COLOR_RUNNING_ACCENT 0x789D66
#define FOCUS_COLOR_RUNNING_PRIMARY 0xFFFDF8
#define FOCUS_COLOR_RUNNING_SECONDARY 0xF3F6E9
#define FOCUS_COLOR_RUNNING_LEFT 0xF3F6E9

#define FOCUS_COLOR_PAUSED_BG 0xFBF8F1
#define FOCUS_COLOR_PAUSED_ACCENT 0xE89A44
#define FOCUS_COLOR_PAUSED_PRIMARY 0xE89A44
#define FOCUS_COLOR_PAUSED_SECONDARY 0xFFFDF8
#define FOCUS_COLOR_PAUSED_LEFT 0xF3F6E9

#define FOCUS_COLOR_FINISHED_BG 0xFBF8F1
#define FOCUS_COLOR_FINISHED_ACCENT 0x789D66
#define FOCUS_COLOR_FINISHED_PRIMARY 0x789D66
#define FOCUS_COLOR_FINISHED_SECONDARY 0xFFFDF8
#define FOCUS_COLOR_FINISHED_LEFT 0xF3F6E9
