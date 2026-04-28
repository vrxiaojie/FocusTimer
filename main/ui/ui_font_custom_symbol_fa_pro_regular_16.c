/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --no-compress --font ..\..\FocusTimer专注时钟--星火计划\UI字体\Font Awesome 6 Pro-Regular-400.otf --symbols  --range 32-127 --format lvgl
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl.h"
#endif

#ifndef UI_FONT_CUSTOM_SYMBOL_FA_PRO_REGULAR_16
#define UI_FONT_CUSTOM_SYMBOL_FA_PRO_REGULAR_16 1
#endif

#if UI_FONT_CUSTOM_SYMBOL_FA_PRO_REGULAR_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+002D "-" */
    0xf8,

    /* U+002E "." */
    0x80,

    /* U+0030 "0" */
    0x3c, 0x42, 0x83, 0x81, 0x81, 0x81, 0x81, 0x81,
    0xc3, 0x42, 0x3c,

    /* U+0031 "1" */
    0x31, 0xc9, 0x4, 0x10, 0x41, 0x4, 0x10, 0x4f,
    0xc0,

    /* U+0032 "2" */
    0x7d, 0x8c, 0x8, 0x10, 0x60, 0x82, 0x8, 0x20,
    0x83, 0xf8,

    /* U+0033 "3" */
    0x3d, 0x8c, 0x8, 0x10, 0x67, 0x81, 0x81, 0x3,
    0x8d, 0xf0,

    /* U+0034 "4" */
    0x4, 0xc, 0x1c, 0x14, 0x24, 0x44, 0xc4, 0xff,
    0x4, 0x4, 0x4,

    /* U+0035 "5" */
    0xff, 0x2, 0x4, 0xf, 0x98, 0x80, 0x81, 0x3,
    0x9, 0xe0,

    /* U+0036 "6" */
    0x3e, 0x63, 0x40, 0x80, 0xbc, 0xc2, 0xc1, 0x81,
    0xc1, 0x42, 0x3c,

    /* U+0037 "7" */
    0xff, 0x3, 0x2, 0x6, 0xc, 0x8, 0x18, 0x10,
    0x30, 0x20, 0x60,

    /* U+0038 "8" */
    0x3c, 0xc3, 0x81, 0x81, 0x42, 0x3c, 0xc3, 0x81,
    0x81, 0xc3, 0x3c,

    /* U+0039 "9" */
    0x3c, 0x42, 0x83, 0x81, 0x83, 0x43, 0x3d, 0x1,
    0x2, 0xc6, 0x7c,

    /* U+0041 "A" */
    0x6, 0x0, 0xc0, 0x2c, 0x4, 0x81, 0x98, 0x21,
    0xc, 0x21, 0xfe, 0x60, 0x48, 0xd, 0x0, 0x80,

    /* U+0042 "B" */
    0xfe, 0x83, 0x81, 0x81, 0x83, 0xfe, 0x83, 0x81,
    0x81, 0x83, 0xfe,

    /* U+0043 "C" */
    0x1f, 0x30, 0xd0, 0x10, 0x8, 0x4, 0x2, 0x1,
    0x0, 0x40, 0x30, 0xc7, 0xc0,

    /* U+0044 "D" */
    0xfc, 0x41, 0xa0, 0x50, 0x18, 0xc, 0x6, 0x3,
    0x1, 0x81, 0x41, 0xbf, 0x0,

    /* U+0045 "E" */
    0xff, 0x2, 0x4, 0x8, 0x1f, 0xe0, 0x40, 0x81,
    0x3, 0xf8,

    /* U+0046 "F" */
    0xff, 0x2, 0x4, 0x8, 0x1f, 0xa0, 0x40, 0x81,
    0x2, 0x0,

    /* U+0047 "G" */
    0x1f, 0x30, 0xd0, 0x10, 0x8, 0x4, 0x3e, 0x3,
    0x1, 0x40, 0xb0, 0x47, 0xc0,

    /* U+0048 "H" */
    0x80, 0xc0, 0x60, 0x30, 0x18, 0xf, 0xfe, 0x3,
    0x1, 0x80, 0xc0, 0x60, 0x20,

    /* U+0049 "I" */
    0xff, 0xe0,

    /* U+004A "J" */
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1e,

    /* U+004B "K" */
    0x83, 0x43, 0x23, 0x12, 0xa, 0x7, 0x2, 0xc1,
    0x30, 0x8c, 0x43, 0x20, 0xc0,

    /* U+004C "L" */
    0x81, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x81,
    0x3, 0xf8,

    /* U+004D "M" */
    0x80, 0x38, 0xf, 0x1, 0xf0, 0x7a, 0xb, 0x63,
    0x64, 0x4c, 0xc9, 0x8a, 0x31, 0xc6, 0x10, 0x80,

    /* U+004E "N" */
    0x80, 0xe0, 0x78, 0x36, 0x19, 0xc, 0x46, 0x13,
    0xd, 0x83, 0xc0, 0xe0, 0x20,

    /* U+004F "O" */
    0x1e, 0x18, 0x64, 0xa, 0x1, 0x80, 0x60, 0x18,
    0x6, 0x1, 0x40, 0x98, 0x61, 0xe0,

    /* U+0050 "P" */
    0xfe, 0x83, 0x81, 0x81, 0x83, 0xfe, 0x80, 0x80,
    0x80, 0x80, 0x80,

    /* U+0051 "Q" */
    0x1e, 0x18, 0x64, 0xa, 0x1, 0x80, 0x60, 0x18,
    0x6, 0x1, 0x40, 0x98, 0x61, 0xe0, 0xc, 0x1,
    0x0, 0x20,

    /* U+0052 "R" */
    0xfe, 0x41, 0xa0, 0x50, 0x28, 0x37, 0xf2, 0x9,
    0x4, 0x83, 0x40, 0xa0, 0x40,

    /* U+0053 "S" */
    0x3e, 0xc3, 0x80, 0x80, 0xc0, 0x3c, 0x6, 0x1,
    0x1, 0x83, 0x7c,

    /* U+0054 "T" */
    0xff, 0x84, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+0055 "U" */
    0x80, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x3,
    0x1, 0x80, 0xa0, 0x8f, 0x80,

    /* U+0056 "V" */
    0xc0, 0x28, 0x9, 0x81, 0x10, 0x62, 0x8, 0x63,
    0x4, 0x40, 0xd8, 0xa, 0x1, 0xc0, 0x10, 0x0,

    /* U+0057 "W" */
    0x81, 0x81, 0x81, 0x83, 0xc3, 0x82, 0x42, 0xc2,
    0x42, 0x46, 0x26, 0x44, 0x24, 0x24, 0x34, 0x2c,
    0x1c, 0x28, 0x18, 0x18, 0x18, 0x10,

    /* U+0058 "X" */
    0x40, 0xd8, 0x63, 0x10, 0x68, 0xe, 0x3, 0x0,
    0xe0, 0x4c, 0x31, 0x18, 0x24, 0xc,

    /* U+0059 "Y" */
    0x80, 0xa0, 0x98, 0xc4, 0x41, 0x40, 0xa0, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+005A "Z" */
    0xff, 0x3, 0x6, 0x4, 0x8, 0x18, 0x10, 0x20,
    0x60, 0xc0, 0xff,

    /* U+0061 "a" */
    0x3a, 0x8e, 0xc, 0x18, 0x30, 0x51, 0x9d,

    /* U+0062 "b" */
    0x81, 0x2, 0x4, 0xb, 0x98, 0xa0, 0xc1, 0x83,
    0x7, 0x15, 0xc0,

    /* U+0063 "c" */
    0x3d, 0x18, 0x20, 0x82, 0x4, 0x4f,

    /* U+0064 "d" */
    0x2, 0x4, 0x8, 0x13, 0xa8, 0xe0, 0xc1, 0x83,
    0x5, 0x19, 0xd0,

    /* U+0065 "e" */
    0x3c, 0x8a, 0xf, 0xf8, 0x10, 0x10, 0x9e,

    /* U+0066 "f" */
    0x19, 0x8, 0x4f, 0x90, 0x84, 0x21, 0x8, 0x40,

    /* U+0067 "g" */
    0x3a, 0x8e, 0xc, 0x18, 0x30, 0x51, 0x9d, 0x2,
    0x5, 0x1b, 0xe0,

    /* U+0068 "h" */
    0x81, 0x2, 0x4, 0xb, 0xd8, 0xe0, 0xc1, 0x83,
    0x6, 0xc, 0x10,

    /* U+0069 "i" */
    0x9f, 0xe0,

    /* U+006A "j" */
    0x20, 0x12, 0x49, 0x24, 0x92, 0x70,

    /* U+006B "k" */
    0x81, 0x2, 0x4, 0x8, 0xd3, 0x2c, 0x70, 0xe1,
    0x62, 0x24, 0x20,

    /* U+006C "l" */
    0xff, 0xf0,

    /* U+006D "m" */
    0xb9, 0xd8, 0xc6, 0x10, 0xc2, 0x18, 0x43, 0x8,
    0x61, 0xc, 0x21,

    /* U+006E "n" */
    0xbd, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xc1,

    /* U+006F "o" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x51, 0x1c,

    /* U+0070 "p" */
    0xb9, 0x8a, 0xc, 0x18, 0x30, 0x71, 0x5c, 0x81,
    0x2, 0x4, 0x0,

    /* U+0071 "q" */
    0x3a, 0x8e, 0xc, 0x18, 0x30, 0x51, 0x9d, 0x2,
    0x4, 0x8, 0x10,

    /* U+0072 "r" */
    0xbe, 0x21, 0x8, 0x42, 0x10,

    /* U+0073 "s" */
    0x7a, 0x18, 0x38, 0x18, 0x18, 0x5e,

    /* U+0074 "t" */
    0x1, 0x9, 0xf2, 0x10, 0x84, 0x21, 0x6,

    /* U+0075 "u" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x71, 0xbd,

    /* U+0076 "v" */
    0xc1, 0x43, 0x62, 0x22, 0x24, 0x14, 0x18, 0x18,

    /* U+0077 "w" */
    0x43, 0xa, 0x18, 0x59, 0xc6, 0x49, 0x22, 0x49,
    0x1e, 0x70, 0x61, 0x83, 0xc,

    /* U+0078 "x" */
    0x43, 0x26, 0x34, 0x18, 0x1c, 0x34, 0x62, 0x43,

    /* U+0079 "y" */
    0xc1, 0x43, 0x62, 0x22, 0x24, 0x14, 0x1c, 0x18,
    0x18, 0x10, 0x60, 0x40,

    /* U+007A "z" */
    0xfe, 0xc, 0x30, 0xc3, 0xc, 0x10, 0x7f,

    /* U+F240 "" */
    0x7f, 0xff, 0x30, 0x0, 0x6c, 0x0, 0x1b, 0x7f,
    0xf7, 0xdf, 0xfd, 0xf7, 0xff, 0x7d, 0xff, 0xdf,
    0x0, 0x6, 0xc0, 0x1, 0x9f, 0xff, 0xc0,

    /* U+F241 "" */
    0x7f, 0xff, 0x30, 0x0, 0x6c, 0x0, 0x1b, 0x7f,
    0x87, 0xdf, 0xe1, 0xf7, 0xf8, 0x7d, 0xfe, 0x1f,
    0x0, 0x6, 0xc0, 0x1, 0x9f, 0xff, 0xc0,

    /* U+F242 "" */
    0x7f, 0xff, 0x30, 0x0, 0x6c, 0x0, 0x1b, 0x7e,
    0x7, 0xdf, 0x81, 0xf7, 0xe0, 0x7d, 0xf8, 0x1f,
    0x0, 0x6, 0xc0, 0x1, 0x9f, 0xff, 0xc0,

    /* U+F243 "" */
    0x7f, 0xff, 0x30, 0x0, 0x6c, 0x0, 0x1b, 0x70,
    0x7, 0xdc, 0x1, 0xf7, 0x0, 0x7d, 0xc0, 0x1f,
    0x0, 0x6, 0xc0, 0x1, 0x9f, 0xff, 0xc0,

    /* U+F244 "" */
    0x7f, 0xff, 0x30, 0x0, 0x6c, 0x0, 0x1b, 0x0,
    0x7, 0xc0, 0x1, 0xf0, 0x0, 0x7c, 0x0, 0x1f,
    0x0, 0x6, 0xc0, 0x1, 0x9f, 0xff, 0xc0,

    /* U+F376 "" */
    0x0, 0x0, 0x0, 0x18, 0x7, 0xc6, 0xf3, 0x3,
    0x86, 0xc1, 0xc1, 0xb0, 0xf0, 0x7c, 0x7c, 0x1f,
    0x7, 0xc7, 0xc1, 0xf1, 0xf0, 0x78, 0x6c, 0x3c,
    0x19, 0xee, 0x7c, 0x3, 0x0, 0x0, 0x0, 0x0
};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 100, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 110, .box_w = 5, .box_h = 1, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 2, .adv_w = 60, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 3, .adv_w = 154, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 14, .adv_w = 168, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = 2},
    {.bitmap_index = 23, .adv_w = 155, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 33, .adv_w = 149, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 43, .adv_w = 158, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 54, .adv_w = 161, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 2},
    {.bitmap_index = 64, .adv_w = 156, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 75, .adv_w = 154, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 86, .adv_w = 154, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 97, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 108, .adv_w = 188, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 124, .adv_w = 182, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 135, .adv_w = 179, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 148, .adv_w = 198, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 161, .adv_w = 160, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 171, .adv_w = 158, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 181, .adv_w = 183, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 194, .adv_w = 196, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 207, .adv_w = 66, .box_w = 1, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 209, .adv_w = 63, .box_w = 4, .box_h = 14, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 216, .adv_w = 188, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 229, .adv_w = 158, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 239, .adv_w = 218, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 255, .adv_w = 191, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 268, .adv_w = 198, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 282, .adv_w = 178, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 293, .adv_w = 198, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 311, .adv_w = 188, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 324, .adv_w = 160, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 335, .adv_w = 155, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 348, .adv_w = 187, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 361, .adv_w = 179, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 377, .adv_w = 285, .box_w = 16, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 399, .adv_w = 167, .box_w = 10, .box_h = 11, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 413, .adv_w = 164, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 426, .adv_w = 155, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 437, .adv_w = 146, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 444, .adv_w = 162, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 455, .adv_w = 133, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 461, .adv_w = 146, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 472, .adv_w = 140, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 479, .adv_w = 85, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 487, .adv_w = 147, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 498, .adv_w = 151, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 509, .adv_w = 61, .box_w = 1, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 511, .adv_w = 57, .box_w = 3, .box_h = 15, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 517, .adv_w = 154, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 528, .adv_w = 61, .box_w = 1, .box_h = 12, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 530, .adv_w = 224, .box_w = 11, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 541, .adv_w = 149, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 548, .adv_w = 147, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 555, .adv_w = 160, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 566, .adv_w = 148, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 577, .adv_w = 110, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 582, .adv_w = 126, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 588, .adv_w = 87, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 595, .adv_w = 147, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 602, .adv_w = 136, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 610, .adv_w = 223, .box_w = 13, .box_h = 8, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 623, .adv_w = 137, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 631, .adv_w = 136, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 643, .adv_w = 135, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 650, .adv_w = 288, .box_w = 18, .box_h = 10, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 673, .adv_w = 288, .box_w = 18, .box_h = 10, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 696, .adv_w = 288, .box_w = 18, .box_h = 10, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 719, .adv_w = 288, .box_w = 18, .box_h = 10, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 742, .adv_w = 288, .box_w = 18, .box_h = 10, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 765, .adv_w = 288, .box_w = 18, .box_h = 14, .ofs_x = 0, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xd, 0xe
};

static const uint16_t unicode_list_4[] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x136
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 15, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 3, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 48, .range_length = 10, .glyph_id_start = 4,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 65, .range_length = 26, .glyph_id_start = 14,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 97, .range_length = 26, .glyph_id_start = 40,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 62016, .range_length = 311, .glyph_id_start = 66,
        .unicode_list = unicode_list_4, .glyph_id_ofs_list = NULL, .list_length = 6, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
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
    .cmap_num = 5,
    .bpp = 1,
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
const lv_font_t ui_font_custom_symbol_fa_pro_regular_16 = {
#else
lv_font_t ui_font_custom_symbol_fa_pro_regular_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
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

#endif /*#if UI_FONT_CUSTOM_SYMBOL_FA_PRO_REGULAR_16*/