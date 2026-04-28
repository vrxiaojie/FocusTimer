/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --no-compress --font ..\..\FocusTimer专注时钟--星火计划\UI字体\siyuanheiti.otf --symbols 0123456789% --range 32-127 --format lvgl
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

#ifndef UI_FONT_SIYUANHEITI_16
#define UI_FONT_SIYUANHEITI_16 1
#endif

#if UI_FONT_SIYUANHEITI_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0x55, 0x55, 0xf,

    /* U+0022 "\"" */
    0x99, 0x99,

    /* U+0023 "#" */
    0x14, 0x24, 0x24, 0x7f, 0x24, 0x24, 0x24, 0xfe,
    0x24, 0x28, 0x48, 0x48,

    /* U+0024 "$" */
    0x10, 0x47, 0xb1, 0x82, 0xc, 0x1c, 0x18, 0x30,
    0x41, 0x8d, 0xe1, 0x4,

    /* U+0025 "%" */
    0x70, 0x46, 0xc4, 0x22, 0x21, 0x12, 0x8, 0x97,
    0x6d, 0x6d, 0xca, 0x20, 0x91, 0x4, 0x88, 0x44,
    0x42, 0x36, 0x20, 0xe0,

    /* U+0026 "&" */
    0x18, 0x12, 0x9, 0x4, 0x82, 0x80, 0x81, 0xc3,
    0x93, 0x85, 0x41, 0xb0, 0xcf, 0x90,

    /* U+0027 "'" */
    0xf0,

    /* U+0028 "(" */
    0x29, 0x29, 0x24, 0x92, 0x44, 0x91,

    /* U+0029 ")" */
    0x89, 0x22, 0x49, 0x24, 0x94, 0x94,

    /* U+002A "*" */
    0x21, 0x3e, 0x65, 0x80,

    /* U+002B "+" */
    0x10, 0x10, 0x10, 0xff, 0x10, 0x10, 0x10, 0x10,

    /* U+002C "," */
    0x6c, 0xb4,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x4, 0x10, 0x82, 0x8, 0x41, 0x4, 0x20, 0x82,
    0x10, 0x41, 0xc, 0x20,

    /* U+0030 "0" */
    0x38, 0x8b, 0x1c, 0x18, 0x30, 0x60, 0xc1, 0x83,
    0x89, 0x11, 0xc0,

    /* U+0031 "1" */
    0x31, 0xc1, 0x4, 0x10, 0x41, 0x4, 0x10, 0x41,
    0x3f,

    /* U+0032 "2" */
    0x79, 0x88, 0x8, 0x10, 0x20, 0xc1, 0x6, 0x18,
    0x61, 0x87, 0xf0,

    /* U+0033 "3" */
    0x79, 0x98, 0x10, 0x20, 0xc7, 0x1, 0x1, 0x2,
    0x7, 0x13, 0xc0,

    /* U+0034 "4" */
    0xc, 0xc, 0x1c, 0x14, 0x34, 0x24, 0x44, 0xc4,
    0xff, 0x4, 0x4, 0x4,

    /* U+0035 "5" */
    0x7e, 0x81, 0x2, 0x7, 0xc8, 0x80, 0x81, 0x2,
    0x7, 0x13, 0xc0,

    /* U+0036 "6" */
    0x3c, 0x81, 0x4, 0xb, 0xd8, 0xa0, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0037 "7" */
    0xfe, 0x8, 0x10, 0x40, 0x81, 0x4, 0x8, 0x10,
    0x20, 0x40, 0x80,

    /* U+0038 "8" */
    0x3c, 0xcd, 0xa, 0x16, 0x27, 0x9b, 0x43, 0x83,
    0x7, 0x19, 0xe0,

    /* U+0039 "9" */
    0x78, 0x8a, 0xc, 0x18, 0x38, 0xde, 0x81, 0x2,
    0xa, 0x33, 0xc0,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+003B ";" */
    0xf0, 0x3, 0xde,

    /* U+003C "<" */
    0x1, 0x7, 0x38, 0xe0, 0xe0, 0x38, 0x7, 0x1,

    /* U+003D "=" */
    0xff, 0x0, 0x0, 0x0, 0xff,

    /* U+003E ">" */
    0x0, 0xe0, 0x38, 0x7, 0x7, 0x38, 0xe0, 0x0,

    /* U+003F "?" */
    0x7b, 0x10, 0x41, 0x8, 0x61, 0xc, 0x0, 0x3,
    0xc,

    /* U+0040 "@" */
    0x7, 0xc0, 0xc3, 0x8, 0x4, 0x80, 0x34, 0x38,
    0xc2, 0x46, 0x22, 0x31, 0x11, 0x88, 0x9c, 0x4c,
    0xb1, 0xb8, 0x80, 0x3, 0x0, 0x7, 0xc0,

    /* U+0041 "A" */
    0xc, 0x3, 0x1, 0x60, 0x58, 0x12, 0xc, 0xc2,
    0x30, 0xfc, 0x61, 0x90, 0x64, 0xb, 0x2,

    /* U+0042 "B" */
    0xfc, 0x86, 0x82, 0x82, 0x86, 0xfc, 0x82, 0x81,
    0x81, 0x81, 0x82, 0xfc,

    /* U+0043 "C" */
    0x1e, 0x63, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x40, 0x63, 0x1e,

    /* U+0044 "D" */
    0xf8, 0x86, 0x82, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x82, 0x86, 0xf8,

    /* U+0045 "E" */
    0xfe, 0x8, 0x20, 0x83, 0xe8, 0x20, 0x82, 0x8,
    0x3f,

    /* U+0046 "F" */
    0xfe, 0x8, 0x20, 0x82, 0xf, 0xa0, 0x82, 0x8,
    0x20,

    /* U+0047 "G" */
    0x1f, 0x10, 0xd0, 0x10, 0x8, 0x4, 0x2, 0x1f,
    0x1, 0xc0, 0xa0, 0x48, 0x23, 0xe0,

    /* U+0048 "H" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81,

    /* U+0049 "I" */
    0xff, 0xf0,

    /* U+004A "J" */
    0x4, 0x10, 0x41, 0x4, 0x10, 0x41, 0x4, 0x1c,
    0xde,

    /* U+004B "K" */
    0x82, 0x84, 0x8c, 0x98, 0xb0, 0xf0, 0xd8, 0xcc,
    0x84, 0x86, 0x82, 0x83,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x82, 0x8,
    0x3f,

    /* U+004D "M" */
    0xc0, 0xf0, 0x3e, 0x1e, 0x85, 0xa1, 0x6c, 0xd9,
    0x26, 0x49, 0x8c, 0x63, 0x18, 0xc6, 0x1,

    /* U+004E "N" */
    0x81, 0xc1, 0xe1, 0xa1, 0xb1, 0x91, 0x89, 0x8d,
    0x85, 0x87, 0x83, 0x81,

    /* U+004F "O" */
    0x1e, 0x18, 0x64, 0xa, 0x1, 0x80, 0x60, 0x18,
    0x6, 0x1, 0x80, 0x50, 0x26, 0x10, 0x78,

    /* U+0050 "P" */
    0xfc, 0x82, 0x81, 0x81, 0x81, 0x82, 0xfc, 0x80,
    0x80, 0x80, 0x80, 0x80,

    /* U+0051 "Q" */
    0x1e, 0x18, 0x44, 0xa, 0x1, 0x80, 0x60, 0x18,
    0x6, 0x1, 0x80, 0x50, 0x26, 0x18, 0xfc, 0xc,
    0x1, 0x80, 0x3c,

    /* U+0052 "R" */
    0xfc, 0x82, 0x81, 0x81, 0x81, 0x82, 0xfc, 0x8c,
    0x84, 0x86, 0x83, 0x81,

    /* U+0053 "S" */
    0x3e, 0x63, 0x40, 0x40, 0x60, 0x3c, 0xe, 0x3,
    0x1, 0x1, 0xe2, 0x3c,

    /* U+0054 "T" */
    0xff, 0x84, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x1, 0x0,

    /* U+0055 "U" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x42, 0x3c,

    /* U+0056 "V" */
    0xc0, 0xe0, 0xd0, 0x4c, 0x26, 0x31, 0x10, 0x88,
    0x64, 0x16, 0xa, 0x7, 0x1, 0x80,

    /* U+0057 "W" */
    0x43, 0xd, 0xc, 0x24, 0x30, 0x98, 0xe2, 0x64,
    0x98, 0x92, 0x62, 0x49, 0x9, 0x34, 0x3c, 0x50,
    0xe1, 0xc1, 0x86, 0x6, 0x18,

    /* U+0058 "X" */
    0x41, 0x31, 0x88, 0x86, 0xc1, 0x40, 0x60, 0x70,
    0x28, 0x36, 0x11, 0x98, 0xc8, 0x30,

    /* U+0059 "Y" */
    0x41, 0x31, 0x88, 0x84, 0x41, 0x40, 0xa0, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x1, 0x0,

    /* U+005A "Z" */
    0xff, 0x2, 0x6, 0x4, 0xc, 0x18, 0x10, 0x30,
    0x20, 0x40, 0xc0, 0xff,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x49, 0x27,

    /* U+005C "\\" */
    0x81, 0x4, 0x10, 0x20, 0x82, 0x4, 0x10, 0x41,
    0x82, 0x8, 0x20, 0x41,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x92, 0x4f,

    /* U+005E "^" */
    0x10, 0x50, 0xa3, 0x44, 0x48, 0xb1, 0x80,

    /* U+005F "_" */
    0xff, 0x80,

    /* U+0060 "`" */
    0x46, 0x30,

    /* U+0061 "a" */
    0x7d, 0x8c, 0x8, 0x37, 0xb8, 0x60, 0xc3, 0x7a,

    /* U+0062 "b" */
    0x81, 0x2, 0x4, 0xb, 0x98, 0xa0, 0xc1, 0x83,
    0x6, 0xe, 0x2b, 0x80,

    /* U+0063 "c" */
    0x3d, 0x18, 0x20, 0x82, 0x8, 0x11, 0x3c,

    /* U+0064 "d" */
    0x2, 0x4, 0x8, 0x13, 0xa8, 0xe0, 0xc1, 0x83,
    0x6, 0xa, 0x33, 0xa0,

    /* U+0065 "e" */
    0x38, 0x8a, 0xc, 0x1f, 0xf0, 0x20, 0x20, 0x3c,

    /* U+0066 "f" */
    0x3a, 0x10, 0x8f, 0x21, 0x8, 0x42, 0x10, 0x84,
    0x0,

    /* U+0067 "g" */
    0x3f, 0x66, 0x42, 0x42, 0x66, 0x7c, 0x40, 0x40,
    0x7e, 0xc1, 0x81, 0xc2, 0x7c,

    /* U+0068 "h" */
    0x81, 0x2, 0x4, 0xb, 0xd8, 0xe0, 0xc1, 0x83,
    0x6, 0xc, 0x18, 0x20,

    /* U+0069 "i" */
    0xcf, 0xf8,

    /* U+006A "j" */
    0x24, 0x2, 0x49, 0x24, 0x92, 0x49, 0xc0,

    /* U+006B "k" */
    0x81, 0x2, 0x4, 0x8, 0xd1, 0x24, 0x58, 0xf1,
    0xb2, 0x24, 0x68, 0x40,

    /* U+006C "l" */
    0xaa, 0xaa, 0xaa, 0xc0,

    /* U+006D "m" */
    0xb9, 0xd9, 0xce, 0x10, 0xc2, 0x18, 0x43, 0x8,
    0x61, 0xc, 0x21, 0x84, 0x20,

    /* U+006E "n" */
    0xbd, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x82,

    /* U+006F "o" */
    0x3c, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81, 0x42,
    0x3c,

    /* U+0070 "p" */
    0xb9, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xe2, 0xf9,
    0x2, 0x4, 0x8, 0x0,

    /* U+0071 "q" */
    0x3e, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xa3, 0x3a,
    0x4, 0x8, 0x10, 0x20,

    /* U+0072 "r" */
    0xbc, 0x88, 0x88, 0x88, 0x80,

    /* U+0073 "s" */
    0x7a, 0x8, 0x30, 0x38, 0x30, 0x61, 0x78,

    /* U+0074 "t" */
    0x21, 0x9, 0xf2, 0x10, 0x84, 0x21, 0x8, 0x30,

    /* U+0075 "u" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x60, 0xe3, 0x7a,

    /* U+0076 "v" */
    0xc1, 0x43, 0x62, 0x62, 0x26, 0x24, 0x34, 0x1c,
    0x18,

    /* U+0077 "w" */
    0x42, 0x14, 0x71, 0x65, 0x36, 0x52, 0x25, 0x22,
    0x9a, 0x28, 0xe3, 0x8c, 0x18, 0xc0,

    /* U+0078 "x" */
    0x42, 0x66, 0x34, 0x18, 0x18, 0x3c, 0x24, 0x66,
    0x42,

    /* U+0079 "y" */
    0xc1, 0x43, 0x62, 0x22, 0x26, 0x34, 0x14, 0x14,
    0x18, 0x8, 0x18, 0x10, 0x60,

    /* U+007A "z" */
    0x7e, 0x8, 0x30, 0xc1, 0x86, 0x18, 0x30, 0xfe,

    /* U+007B "{" */
    0x74, 0x44, 0x44, 0x48, 0x44, 0x44, 0x44, 0x47,

    /* U+007C "|" */
    0xff, 0xff, 0xc0,

    /* U+007D "}" */
    0xe2, 0x22, 0x22, 0x21, 0x22, 0x22, 0x22, 0x2c,

    /* U+007E "~" */
    0x63, 0x26, 0x30
};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 57, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 83, .box_w = 2, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 121, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 6, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 18, .adv_w = 142, .box_w = 6, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 30, .adv_w = 236, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 50, .adv_w = 174, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 64, .adv_w = 71, .box_w = 1, .box_h = 4, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 65, .adv_w = 87, .box_w = 3, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 71, .adv_w = 87, .box_w = 3, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 77, .adv_w = 120, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 81, .adv_w = 142, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 89, .adv_w = 71, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 91, .adv_w = 89, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 92, .adv_w = 71, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 93, .adv_w = 100, .box_w = 6, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 105, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 116, .adv_w = 142, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 125, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 136, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 147, .adv_w = 142, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 170, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 181, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 192, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 203, .adv_w = 142, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 214, .adv_w = 71, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 217, .adv_w = 71, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 220, .adv_w = 142, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 228, .adv_w = 142, .box_w = 8, .box_h = 5, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 233, .adv_w = 142, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 241, .adv_w = 121, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 242, .box_w = 13, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 273, .adv_w = 156, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 288, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 300, .adv_w = 163, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 312, .adv_w = 176, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 324, .adv_w = 151, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 333, .adv_w = 141, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 342, .adv_w = 176, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 356, .adv_w = 186, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 368, .adv_w = 75, .box_w = 1, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 370, .adv_w = 137, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 379, .adv_w = 165, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 391, .adv_w = 139, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 400, .adv_w = 208, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 415, .adv_w = 185, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 427, .adv_w = 190, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 442, .adv_w = 162, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 454, .adv_w = 190, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 473, .adv_w = 163, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 485, .adv_w = 153, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 497, .adv_w = 153, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 511, .adv_w = 185, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 523, .adv_w = 147, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 537, .adv_w = 225, .box_w = 14, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 558, .adv_w = 147, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 572, .adv_w = 136, .box_w = 9, .box_h = 12, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 586, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 598, .adv_w = 87, .box_w = 3, .box_h = 16, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 604, .adv_w = 100, .box_w = 6, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 616, .adv_w = 87, .box_w = 3, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 622, .adv_w = 142, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 629, .adv_w = 143, .box_w = 9, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 631, .adv_w = 155, .box_w = 4, .box_h = 4, .ofs_x = 2, .ofs_y = 10},
    {.bitmap_index = 633, .adv_w = 144, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 641, .adv_w = 158, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 653, .adv_w = 131, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 660, .adv_w = 159, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 672, .adv_w = 142, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 680, .adv_w = 83, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 689, .adv_w = 144, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 702, .adv_w = 155, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 714, .adv_w = 70, .box_w = 1, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 716, .adv_w = 70, .box_w = 3, .box_h = 17, .ofs_x = -1, .ofs_y = -4},
    {.bitmap_index = 723, .adv_w = 141, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 735, .adv_w = 73, .box_w = 2, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 739, .adv_w = 237, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 752, .adv_w = 156, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 760, .adv_w = 155, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 769, .adv_w = 159, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 781, .adv_w = 159, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 793, .adv_w = 99, .box_w = 4, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 798, .adv_w = 120, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 805, .adv_w = 97, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 813, .adv_w = 155, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 821, .adv_w = 133, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 830, .adv_w = 205, .box_w = 12, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 844, .adv_w = 127, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 853, .adv_w = 133, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 866, .adv_w = 122, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 874, .adv_w = 87, .box_w = 4, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 882, .adv_w = 69, .box_w = 1, .box_h = 18, .ofs_x = 2, .ofs_y = -5},
    {.bitmap_index = 885, .adv_w = 87, .box_w = 4, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 893, .adv_w = 142, .box_w = 7, .box_h = 3, .ofs_x = 1, .ofs_y = 5}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/

/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 1, 0, 0, 0, 0,
    1, 2, 0, 0, 0, 3, 4, 3,
    5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 6, 6, 0, 0, 0,
    0, 0, 7, 8, 9, 10, 11, 12,
    13, 0, 0, 14, 15, 16, 0, 0,
    10, 17, 10, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 2, 27, 0, 0,
    0, 0, 28, 29, 30, 0, 31, 32,
    33, 34, 0, 0, 35, 36, 34, 34,
    29, 29, 37, 38, 39, 40, 37, 41,
    42, 43, 44, 45, 2, 0, 0, 0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 1, 2, 0, 0, 0, 0,
    2, 0, 3, 4, 0, 5, 6, 7,
    8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 9, 10, 0, 0, 0,
    11, 0, 12, 0, 13, 0, 0, 0,
    13, 0, 0, 14, 0, 0, 0, 0,
    13, 0, 13, 0, 15, 16, 17, 18,
    19, 20, 21, 22, 0, 23, 3, 0,
    0, 0, 24, 0, 25, 25, 25, 26,
    27, 0, 28, 29, 0, 0, 30, 30,
    25, 30, 25, 30, 31, 32, 33, 34,
    35, 36, 37, 38, 0, 0, 3, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, -33, 0, -33, 0,
    0, 0, 0, -16, 0, -27, -3, 0,
    0, 0, 0, -3, 0, 0, 0, 0,
    -8, 0, 0, 0, 0, 0, -6, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 23, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -27, 0, -39,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -28, -6, -19, -10, 0,
    -26, 0, 0, 0, -3, 0, 0, 0,
    7, 0, 0, -13, 0, -10, -6, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    -5, -14, 0, -5, -3, -8, -19, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -7, 0, -2, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -12, -3, -23, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -7,
    -9, 0, -3, 7, 7, 0, 0, 3,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, -14, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -16, 0, -27,
    0, 0, 0, 0, 0, 0, -7, -2,
    -3, 0, 0, -16, -4, -4, 0, 1,
    -4, -2, -12, 7, 0, -3, 0, 0,
    0, 0, 7, -4, -2, -2, -1, -1,
    -2, 0, 0, 0, 0, -9, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    -4, -7, 0, -2, -1, -1, -4, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -4, -3, -3, -4, 0,
    0, 0, 0, 0, 0, -7, 0, 0,
    0, 0, 0, 0, -8, -3, -7, -5,
    -4, -1, -1, -1, -2, -3, 0, 0,
    0, 0, -6, 0, 0, 0, 0, -8,
    -3, -4, -3, 0, -4, 0, 0, 0,
    0, -10, 0, 0, 0, -5, 0, 0,
    0, -3, 0, -11, 0, -7, 0, -3,
    -2, -5, -6, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, -7, 0, -3, 0, -9,
    -3, 0, 0, 0, 0, 0, -21, 0,
    -21, -20, 0, 0, 0, -11, -3, -40,
    -6, 0, 0, 1, 1, -7, 0, -9,
    0, -10, -4, 0, -7, 0, 0, -6,
    -6, -3, -5, -6, -5, -7, -5, -8,
    0, 0, 0, -8, 0, 0, 0, 0,
    0, 0, 0, -1, 0, 0, 0, -6,
    0, -4, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, 0, -7, 0, 0, 0,
    0, 0, 0, -12, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, -12,
    0, -8, 0, 0, 0, 0, -2, -3,
    -6, 0, -3, -5, -4, -4, -3, 0,
    -5, 0, 0, 0, -2, 0, 0, 0,
    -3, 0, 0, -9, -4, -6, -5, -5,
    -6, -4, 0, -25, 0, -44, 0, -15,
    0, 0, 0, 0, -9, 1, -7, 0,
    -6, -34, -8, -22, -16, 0, -22, 0,
    -23, 0, -3, -4, -1, 0, 0, 0,
    0, -6, -3, -10, -10, 0, -10, 0,
    0, 0, 0, 0, -32, -9, -32, -22,
    0, 0, 0, -14, 0, -42, -3, -7,
    0, 0, 0, -7, -3, -22, 0, -12,
    -7, 0, -9, 0, 0, 0, -3, 0,
    0, 0, 0, -4, 0, -6, 0, 0,
    0, -3, 0, -9, 0, 0, 0, 0,
    0, -1, 0, -5, -4, -4, 0, 2,
    2, -1, 0, -3, 0, -1, -3, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, -2, 0, 0, 0, -5,
    0, 4, 0, 0, 0, 0, 0, 0,
    0, -4, -4, -6, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -30, -21,
    -30, -26, -6, -6, 0, -12, -7, -36,
    -11, 0, 0, 0, 0, -6, -4, -15,
    0, -21, -19, -5, -21, 0, 0, -13,
    -17, -5, -13, -9, -10, -11, -9, -22,
    0, 0, 0, 0, -5, 0, -5, -9,
    0, 0, 0, -5, 0, -14, -3, 0,
    0, -1, 0, -3, -4, 0, 0, -1,
    0, 0, -3, 0, 0, 0, -1, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, -19, -5, -19, -13, 0, 0,
    0, -4, -3, -21, -3, 0, -3, 3,
    0, 0, 0, -5, 0, -6, -4, 0,
    -6, 0, 0, -6, -4, 0, -9, -3,
    -3, -4, -3, -7, 0, 0, 0, 0,
    -10, -3, -10, -8, 0, 0, 0, 0,
    -2, -19, -2, 0, 0, 0, 0, 0,
    0, -2, 0, -5, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -3, 0, -3, 0, -8,
    0, 0, 0, 0, 0, 1, -5, 0,
    -4, -6, -3, 0, 0, 0, 0, 0,
    0, -3, -2, -5, 0, 0, 0, 0,
    0, -5, -3, -5, -4, -3, -5, -4,
    0, 0, 0, 0, -26, -19, -26, -19,
    -7, -7, -2, -4, -4, -29, -5, -4,
    -3, 0, 0, 0, 0, -7, 0, -19,
    -12, 0, -17, 0, 0, -12, -12, -8,
    -10, -4, -7, -10, -4, -14, 0, 0,
    0, 0, 0, -10, 0, 0, 0, 0,
    0, -2, -6, -9, -9, 0, -3, -2,
    -2, 0, -4, -5, 0, -5, -6, -6,
    -4, 0, 0, 0, 0, -4, -7, -5,
    -5, -7, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -25, -8, -15, -8, 0,
    -21, 0, 0, 0, 0, 0, 9, 0,
    21, 0, 0, 0, 0, -6, -3, 0,
    4, 0, 0, 0, 0, -15, 0, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, -7, 0, -5, -1, 0, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, -8, 0, -7, -3, 2, -3, 0,
    0, 0, -4, 0, 0, 0, 0, -16,
    0, -5, 0, -1, -13, 0, -7, -4,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, -1, -1, -5, -1, -2, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, -4,
    0, 0, -7, 0, 0, -3, -6, 0,
    -3, 0, 0, 0, 0, -3, 0, 2,
    2, 2, 2, 0, 0, 0, 0, -10,
    0, 3, 0, 0, 0, 0, -2, 0,
    0, -6, -6, -7, 0, -5, -3, 0,
    -7, 0, -6, -4, 0, 0, -3, 0,
    0, 0, 0, -3, 0, 1, 1, -2,
    1, 0, 4, 12, 14, 0, -14, -4,
    -14, -4, 0, 0, 7, 0, 0, 0,
    0, 13, 0, 19, 13, 9, 17, 0,
    18, -6, -3, 0, -4, 0, -3, 0,
    -1, 0, 0, 4, 0, -1, 0, -4,
    0, 0, 4, -9, 0, 0, 0, 14,
    0, 0, -10, 0, 0, 0, 0, -7,
    0, 0, 0, 0, -4, 0, 0, -5,
    -4, 0, 0, 0, 10, 0, 0, 0,
    0, -1, -1, 0, 4, -4, 0, 0,
    0, -10, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, -7, 0, -3,
    0, 0, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    4, -13, 4, 0, 4, 4, -4, 0,
    0, 0, 0, -10, 0, 0, 0, 0,
    -3, 0, 0, -3, -5, 0, -3, 0,
    -3, 0, 0, -6, -4, 0, 0, -2,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    0, -4, 0, 0, -9, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -16, -7, -16, -10, 7, 7,
    0, -4, 0, -16, 0, 0, 0, 0,
    0, 0, 0, -3, 4, -7, -3, 0,
    -3, 0, 0, 0, -1, 0, 0, 7,
    5, 0, 7, -1, 0, 0, 0, -14,
    0, 3, 0, 0, 0, 0, -3, 0,
    0, 0, 0, -7, 0, -3, 0, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, -7,
    2, 3, 4, 4, -7, 0, 0, 0,
    0, -4, 0, 0, 0, 0, -1, 0,
    0, -6, -4, 0, -3, 0, 0, 0,
    -3, -6, 0, 0, 0, -5, 0, 0,
    0, 0, 0, -3, -10, -2, -10, -6,
    0, 0, 0, -3, 0, -12, 0, -6,
    0, -3, 0, 0, -4, -3, 0, -6,
    -1, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, -7, 0, 0,
    0, -3, -12, 0, -12, -2, 0, 0,
    0, -1, 0, -9, 0, -7, 0, -3,
    0, -4, -7, 0, 0, -3, -1, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, -5, -4, 0, 0, -6,
    2, -4, -2, 0, 0, 2, 0, 0,
    -3, 0, -1, -9, 0, -4, 0, -3,
    -10, 0, 0, -3, -5, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -10, 0,
    -10, -4, 0, 0, 0, 0, 0, -12,
    0, -6, 0, -1, 0, -1, -2, 0,
    0, -6, -1, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, -4, 0, -7,
    0, 0, 0, 0, 0, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -7,
    0, 0, 0, 0, -8, 0, 0, -7,
    -3, 0, -2, 0, 0, 0, 0, 0,
    -3, -1, 0, 0, -1, 0
};

/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 45,
    .right_class_cnt     = 38,
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
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 1,
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
const lv_font_t ui_font_siyuanheiti_16 = {
#else
lv_font_t ui_font_siyuanheiti_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 19,          /*The maximum line height required by the font*/
    .base_line = 5,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};

#endif /*#if UI_FONT_SIYUANHEITI_16*/