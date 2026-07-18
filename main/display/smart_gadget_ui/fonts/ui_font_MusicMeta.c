/*******************************************************************************
 * Size: 10 px
 * Bpp: 4
 * Opts: --font managed_components\lvgl__lvgl\scripts\built_in_font\Montserrat-Medium.ttf -r 0x2D,0x30-0x3A --format lvgl --lv-include lvgl.h --bpp 4 --size 10 --no-compress --no-prefilter --no-kerning --lv-font-name ui_font_MusicMeta -o main\display\smart_gadget_ui\fonts\ui_font_MusicMeta.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl.h"
#endif

#ifndef UI_FONT_MUSICMETA
#define UI_FONT_MUSICMETA 1
#endif

#if UI_FONT_MUSICMETA

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+002D "-" */
    0x5c, 0xc3,

    /* U+0030 "0" */
    0x4, 0xdd, 0xb1, 0x1, 0xe2, 0x6, 0xb0, 0x69,
    0x0, 0xe, 0x17, 0x80, 0x0, 0xd2, 0x69, 0x0,
    0xe, 0x11, 0xe2, 0x6, 0xb0, 0x4, 0xdd, 0xb1,
    0x0,

    /* U+0031 "1" */
    0xbe, 0xa0, 0x5a, 0x5, 0xa0, 0x5a, 0x5, 0xa0,
    0x5a, 0x5, 0xa0,

    /* U+0032 "2" */
    0x4c, 0xdd, 0x50, 0x42, 0x1, 0xf0, 0x0, 0x0,
    0xf0, 0x0, 0xa, 0x80, 0x0, 0xa9, 0x0, 0xb,
    0x80, 0x0, 0x8f, 0xdd, 0xd5,

    /* U+0033 "3" */
    0x8d, 0xde, 0xe0, 0x0, 0xc, 0x40, 0x0, 0x98,
    0x0, 0x0, 0xbd, 0x90, 0x0, 0x0, 0xd3, 0x51,
    0x1, 0xe2, 0x6d, 0xdd, 0x60,

    /* U+0034 "4" */
    0x0, 0x7, 0xa0, 0x0, 0x5, 0xc0, 0x0, 0x3,
    0xd1, 0x31, 0x1, 0xd2, 0xb, 0x30, 0x8d, 0xcc,
    0xfd, 0x70, 0x0, 0xb, 0x30, 0x0, 0x0, 0xb3,
    0x0,

    /* U+0035 "5" */
    0xf, 0xdd, 0xd0, 0x1d, 0x0, 0x0, 0x2c, 0x0,
    0x0, 0x3e, 0xdc, 0x60, 0x0, 0x1, 0xd4, 0x31,
    0x0, 0xc4, 0x5c, 0xdd, 0x80,

    /* U+0036 "6" */
    0x2, 0xbd, 0xd4, 0x1e, 0x40, 0x0, 0x6a, 0x0,
    0x0, 0x7a, 0xab, 0xa1, 0x6e, 0x10, 0x5c, 0x1d,
    0x0, 0x3c, 0x4, 0xcc, 0xb2,

    /* U+0037 "7" */
    0xbd, 0xdd, 0xe8, 0xb4, 0x0, 0xd3, 0x0, 0x4,
    0xc0, 0x0, 0xc, 0x40, 0x0, 0x3d, 0x0, 0x0,
    0xa6, 0x0, 0x1, 0xe0, 0x0,

    /* U+0038 "8" */
    0x7, 0xcc, 0xb2, 0x3d, 0x0, 0x6a, 0x2d, 0x0,
    0x79, 0xb, 0xec, 0xf2, 0x6a, 0x0, 0x4d, 0x79,
    0x0, 0x3e, 0x9, 0xcb, 0xc4,

    /* U+0039 "9" */
    0x1a, 0xcc, 0x60, 0x96, 0x0, 0xb3, 0x97, 0x0,
    0xc9, 0x9, 0xbb, 0x8a, 0x0, 0x0, 0x88, 0x0,
    0x2, 0xe2, 0x2d, 0xdc, 0x40,

    /* U+003A ":" */
    0x6a, 0x1, 0x0, 0x2, 0x6a
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 61, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 2, .adv_w = 107, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 27, .adv_w = 59, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 38, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 80, .adv_w = 107, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 126, .adv_w = 99, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 147, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 168, .adv_w = 103, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 189, .adv_w = 99, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 36, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint8_t glyph_id_ofs_list_0[] = {
    0, 0, 0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 11
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 45, .range_length = 14, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = glyph_id_ofs_list_0, .list_length = 14, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ui_font_MusicMeta = {
#else
lv_font_t ui_font_MusicMeta = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 7,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_MUSICMETA*/

