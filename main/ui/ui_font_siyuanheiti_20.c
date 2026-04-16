/*******************************************************************************
 * Size: 20 px
 * Bpp: 1
 * Opts: --bpp 1 --size 20 --no-compress --font Z:\siyuanheiti.otf --symbols ℃ --range 32-127 --format lvgl
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

#ifndef UI_FONT_SIYUANHEITI_20
#define UI_FONT_SIYUANHEITI_20 1
#endif

#if UI_FONT_SIYUANHEITI_20

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xf0, 0x30,

    /* U+0022 "\"" */
    0xcf, 0x3c, 0xf3, 0xcc,

    /* U+0023 "#" */
    0x11, 0x4, 0x41, 0x11, 0xff, 0x13, 0x4, 0x83,
    0x20, 0x88, 0xff, 0x88, 0x82, 0x20, 0x88, 0x22,
    0x8, 0x80,

    /* U+0024 "$" */
    0xc, 0x6, 0xf, 0xc6, 0x26, 0x3, 0x1, 0x80,
    0xe0, 0x3c, 0xf, 0x1, 0xc0, 0x70, 0x18, 0xc,
    0x6, 0xc6, 0x3f, 0x6, 0x3, 0x0,

    /* U+0025 "%" */
    0x38, 0x8, 0x36, 0x8, 0x31, 0x84, 0x18, 0xc4,
    0xc, 0x64, 0x6, 0x32, 0x71, 0xb2, 0x6c, 0x71,
    0x63, 0x1, 0x31, 0x81, 0x18, 0xc0, 0x8c, 0x60,
    0x86, 0x30, 0x41, 0xb0, 0x40, 0x70,

    /* U+0026 "&" */
    0x1e, 0x3, 0x30, 0x33, 0x3, 0x30, 0x36, 0x1,
    0x40, 0x18, 0x3, 0x83, 0x64, 0x2c, 0x26, 0xc3,
    0x4c, 0x1c, 0x61, 0xe3, 0xe3,

    /* U+0027 "'" */
    0xff, 0xc0,

    /* U+0028 "(" */
    0x11, 0x88, 0xc6, 0x23, 0x18, 0xc6, 0x31, 0x8c,
    0x61, 0x8c, 0x61, 0x8c, 0x20,

    /* U+0029 ")" */
    0xc3, 0x8, 0x63, 0x18, 0x63, 0x18, 0xc6, 0x31,
    0x8c, 0xc6, 0x33, 0x18, 0x80,

    /* U+002A "*" */
    0x10, 0x23, 0xf9, 0xc3, 0x8d, 0x80,

    /* U+002B "+" */
    0xc, 0x3, 0x0, 0xc0, 0x30, 0xc, 0x3f, 0xf0,
    0xc0, 0x30, 0xc, 0x3, 0x0,

    /* U+002C "," */
    0xf3, 0x7e, 0x80,

    /* U+002D "-" */
    0xf8,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0x2, 0x2, 0x6, 0x4, 0x4, 0xc, 0x8, 0x8,
    0x8, 0x18, 0x10, 0x10, 0x30, 0x20, 0x20, 0x60,
    0x40, 0x40, 0xc0,

    /* U+0030 "0" */
    0x3c, 0x31, 0x98, 0xd8, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x6c, 0x66, 0x31, 0xf0,

    /* U+0031 "1" */
    0x38, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0x18, 0xff,

    /* U+0032 "2" */
    0x3e, 0x61, 0x80, 0x60, 0x30, 0x18, 0xc, 0xc,
    0x6, 0x6, 0x7, 0x7, 0x7, 0x7, 0x7, 0xfc,

    /* U+0033 "3" */
    0x3e, 0x31, 0x80, 0x60, 0x30, 0x18, 0x18, 0x70,
    0x6, 0x1, 0x80, 0xc0, 0x60, 0x3c, 0x31, 0xf0,

    /* U+0034 "4" */
    0x7, 0x1, 0xc0, 0xf0, 0x2c, 0x1b, 0xc, 0xc2,
    0x31, 0x8c, 0xc3, 0x3f, 0xf0, 0x30, 0xc, 0x3,
    0x0, 0xc0,

    /* U+0035 "5" */
    0x7f, 0x30, 0x18, 0xc, 0x4, 0x3, 0xf1, 0xc,
    0x3, 0x1, 0x80, 0xc0, 0x70, 0x7c, 0x33, 0xf0,

    /* U+0036 "6" */
    0x1f, 0x18, 0x98, 0x8, 0xc, 0x6, 0xf3, 0x8d,
    0x83, 0xc1, 0xe0, 0xf0, 0x6c, 0x36, 0x30, 0xf0,

    /* U+0037 "7" */
    0xff, 0x80, 0x80, 0xc0, 0xc0, 0x60, 0x60, 0x30,
    0x18, 0x18, 0xc, 0x6, 0x3, 0x1, 0x80, 0xc0,

    /* U+0038 "8" */
    0x1e, 0x19, 0x98, 0x6c, 0x36, 0x19, 0x98, 0x78,
    0x6e, 0x63, 0x60, 0xf0, 0x78, 0x36, 0x31, 0xf0,

    /* U+0039 "9" */
    0x3c, 0x31, 0xb0, 0xd8, 0x3c, 0x1e, 0xd, 0x8e,
    0x7b, 0x1, 0x80, 0xc0, 0xc0, 0x64, 0x63, 0xe0,

    /* U+003A ":" */
    0xfc, 0x0, 0xc,

    /* U+003B ";" */
    0xdb, 0x0, 0x0, 0x3, 0xb7, 0xa0,

    /* U+003C "<" */
    0x0, 0x40, 0xf0, 0xf1, 0xe0, 0xc0, 0x3c, 0x1,
    0xe0, 0x1e, 0x0, 0xc0,

    /* U+003D "=" */
    0xff, 0xc0, 0x0, 0x0, 0x0, 0x0, 0x3f, 0xf0,

    /* U+003E ">" */
    0x80, 0x38, 0x3, 0xc0, 0x1c, 0x1, 0xc1, 0xe1,
    0xe3, 0xc0, 0xc0, 0x0,

    /* U+003F "?" */
    0x79, 0x98, 0x18, 0x30, 0x61, 0x83, 0xc, 0x10,
    0x60, 0xc0, 0x0, 0x6, 0x0,

    /* U+0040 "@" */
    0x3, 0xf8, 0x7, 0x6, 0x6, 0x1, 0x86, 0x0,
    0x66, 0x0, 0x1b, 0xe, 0x8f, 0x18, 0xc7, 0x8c,
    0x43, 0xcc, 0x21, 0xe6, 0x10, 0xf3, 0x18, 0xd9,
    0x8c, 0xc6, 0x7b, 0xc3, 0x0, 0x0, 0xc0, 0x0,
    0x38, 0x20, 0x7, 0xe0, 0x0,

    /* U+0041 "A" */
    0x6, 0x0, 0x70, 0xf, 0x0, 0x90, 0x19, 0x81,
    0x98, 0x10, 0x83, 0xc, 0x3f, 0xc2, 0x4, 0x60,
    0x66, 0x6, 0x40, 0x3c, 0x3,

    /* U+0042 "B" */
    0xfe, 0x30, 0xcc, 0x1b, 0x6, 0xc1, 0xb0, 0xcf,
    0xe3, 0x6, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xc1,
    0xbf, 0xc0,

    /* U+0043 "C" */
    0xf, 0x87, 0x19, 0x80, 0x30, 0xc, 0x1, 0x80,
    0x30, 0x6, 0x0, 0xc0, 0x18, 0x1, 0x80, 0x30,
    0x3, 0x8e, 0x1f, 0x0,

    /* U+0044 "D" */
    0xfe, 0x18, 0x73, 0x3, 0x60, 0x6c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0xf, 0x3, 0x60,
    0x6c, 0x39, 0xfc, 0x0,

    /* U+0045 "E" */
    0xff, 0xe0, 0x30, 0x18, 0xc, 0x6, 0x3, 0xfd,
    0x80, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x7, 0xfc,

    /* U+0046 "F" */
    0xff, 0xe0, 0x30, 0x18, 0xc, 0x6, 0x3, 0xfd,
    0x80, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x0,

    /* U+0047 "G" */
    0xf, 0x87, 0x19, 0x80, 0x30, 0xc, 0x1, 0x80,
    0x30, 0x6, 0x1f, 0xc0, 0x78, 0xd, 0x81, 0xb0,
    0x33, 0xe, 0x1f, 0x80,

    /* U+0048 "H" */
    0xc0, 0x78, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xff, 0xfe, 0x3, 0xc0, 0x78, 0xf, 0x1, 0xe0,
    0x3c, 0x7, 0x80, 0xc0,

    /* U+0049 "I" */
    0xff, 0xff, 0xff, 0xf0,

    /* U+004A "J" */
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0x3, 0xc6, 0x7c,

    /* U+004B "K" */
    0xc0, 0xd8, 0x33, 0xc, 0x63, 0xc, 0xc1, 0xb8,
    0x3f, 0x7, 0xb0, 0xe3, 0x18, 0x63, 0x6, 0x60,
    0xcc, 0xd, 0x80, 0xc0,

    /* U+004C "L" */
    0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x3, 0x1,
    0x80, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x7, 0xfc,

    /* U+004D "M" */
    0xe0, 0x7e, 0x7, 0xe0, 0x7f, 0xf, 0xd0, 0xbd,
    0xb, 0xd9, 0xbc, 0x93, 0xc9, 0x3c, 0xf3, 0xc6,
    0x3c, 0x63, 0xc6, 0x3c, 0x3,

    /* U+004E "N" */
    0xc0, 0x7c, 0xf, 0xc1, 0xe8, 0x3d, 0x87, 0x98,
    0xf3, 0x1e, 0x33, 0xc2, 0x78, 0x6f, 0x7, 0xe0,
    0xfc, 0xf, 0x80, 0xc0,

    /* U+004F "O" */
    0xf, 0x81, 0xc7, 0x18, 0xc, 0xc0, 0x6c, 0x1,
    0xe0, 0xf, 0x0, 0x78, 0x3, 0xc0, 0x1e, 0x0,
    0xd8, 0xc, 0xc0, 0x63, 0x8e, 0x7, 0xc0,

    /* U+0050 "P" */
    0xff, 0x30, 0x6c, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0x1b, 0xfc, 0xc0, 0x30, 0xc, 0x3, 0x0, 0xc0,
    0x30, 0x0,

    /* U+0051 "Q" */
    0xf, 0x81, 0xc7, 0x18, 0xc, 0xc0, 0x6c, 0x1,
    0xe0, 0xf, 0x0, 0x78, 0x3, 0xc0, 0x1e, 0x0,
    0xd8, 0xc, 0xe0, 0xe3, 0xde, 0x7, 0xe0, 0xc,
    0x0, 0x30, 0x0, 0xf8,

    /* U+0052 "R" */
    0xff, 0x30, 0x6c, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0x1b, 0xfc, 0xc6, 0x30, 0xcc, 0x33, 0x6, 0xc1,
    0xb0, 0x30,

    /* U+0053 "S" */
    0x1f, 0xc, 0x66, 0x1, 0x80, 0x70, 0x1f, 0x3,
    0xf0, 0x3e, 0x3, 0xc0, 0x30, 0xd, 0x3, 0xe1,
    0x8f, 0xc0,

    /* U+0054 "T" */
    0xff, 0xe1, 0x80, 0x30, 0x6, 0x0, 0xc0, 0x18,
    0x3, 0x0, 0x60, 0xc, 0x1, 0x80, 0x30, 0x6,
    0x0, 0xc0, 0x18, 0x0,

    /* U+0055 "U" */
    0xc0, 0x78, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0xf, 0x1, 0xb0,
    0x67, 0x1c, 0x3e, 0x0,

    /* U+0056 "V" */
    0xc0, 0x64, 0x6, 0x60, 0x66, 0x4, 0x20, 0xc3,
    0xc, 0x30, 0x81, 0x18, 0x19, 0x81, 0x90, 0xb,
    0x0, 0xf0, 0xe, 0x0, 0x60,

    /* U+0057 "W" */
    0xc1, 0x83, 0xc1, 0x83, 0xc3, 0x82, 0xc2, 0xc2,
    0xc2, 0xc6, 0x62, 0x46, 0x66, 0x46, 0x66, 0x64,
    0x64, 0x64, 0x2c, 0x2c, 0x3c, 0x2c, 0x38, 0x3c,
    0x38, 0x38, 0x38, 0x18,

    /* U+0058 "X" */
    0x60, 0x6e, 0x18, 0xc2, 0xc, 0xc1, 0xb0, 0x1e,
    0x3, 0x80, 0x70, 0xb, 0x3, 0x20, 0xc6, 0x18,
    0x66, 0xc, 0xc0, 0xc0,

    /* U+0059 "Y" */
    0xc0, 0xf0, 0x36, 0x19, 0x86, 0x33, 0xc, 0xc1,
    0xe0, 0x78, 0xc, 0x3, 0x0, 0xc0, 0x30, 0xc,
    0x3, 0x0,

    /* U+005A "Z" */
    0x7f, 0xc0, 0x70, 0x18, 0xc, 0x3, 0x1, 0x80,
    0xc0, 0x30, 0x18, 0xe, 0x3, 0x1, 0x80, 0x60,
    0x3f, 0xf0,

    /* U+005B "[" */
    0xfe, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31, 0x8c,
    0x63, 0x18, 0xc6, 0x3e,

    /* U+005C "\\" */
    0xc0, 0x40, 0x40, 0x60, 0x20, 0x20, 0x30, 0x10,
    0x10, 0x18, 0x8, 0x8, 0xc, 0x4, 0x4, 0x6,
    0x6, 0x2, 0x2,

    /* U+005D "]" */
    0xf8, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0x8c, 0x63, 0x18, 0xfe,

    /* U+005E "^" */
    0x8, 0xe, 0x5, 0x2, 0xc3, 0x61, 0x11, 0x8c,
    0xc2, 0x41, 0x80,

    /* U+005F "_" */
    0xff, 0xe0,

    /* U+0060 "`" */
    0xce, 0x73,

    /* U+0061 "a" */
    0x3e, 0x31, 0x80, 0x60, 0x30, 0xfb, 0xef, 0x87,
    0x83, 0xc1, 0xf1, 0xdf, 0x60,

    /* U+0062 "b" */
    0xc0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x37, 0xce,
    0x1b, 0x6, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xc0,
    0xf0, 0x6e, 0x3b, 0x78,

    /* U+0063 "c" */
    0x1f, 0x38, 0x98, 0x18, 0xc, 0x6, 0x3, 0x1,
    0x80, 0x60, 0x38, 0x87, 0xc0,

    /* U+0064 "d" */
    0x0, 0xc0, 0x30, 0xc, 0x3, 0x0, 0xc7, 0xb7,
    0x1d, 0x83, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xc0,
    0xd8, 0x36, 0x1c, 0xfb,

    /* U+0065 "e" */
    0x1e, 0x31, 0x98, 0x78, 0x3c, 0x1f, 0xff, 0x1,
    0x80, 0x60, 0x30, 0x87, 0xc0,

    /* U+0066 "f" */
    0x3e, 0xe1, 0x83, 0x6, 0x1f, 0x98, 0x30, 0x60,
    0xc1, 0x83, 0x6, 0xc, 0x18, 0x30,

    /* U+0067 "g" */
    0x3f, 0xd8, 0xcc, 0x1b, 0x6, 0xc1, 0xb0, 0x66,
    0x30, 0xf8, 0xc0, 0x30, 0x3, 0xf9, 0x3, 0xc0,
    0xf0, 0x3e, 0x18, 0xfc,

    /* U+0068 "h" */
    0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0xfb, 0x8d,
    0x83, 0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf,
    0x7, 0x83,

    /* U+0069 "i" */
    0xc0, 0x3f, 0xff, 0xff,

    /* U+006A "j" */
    0x30, 0x0, 0x3, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0xe0,

    /* U+006B "k" */
    0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x1b, 0x1d,
    0x8c, 0xcc, 0x6c, 0x3f, 0x1c, 0x8e, 0x66, 0x1b,
    0xd, 0x83,

    /* U+006C "l" */
    0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 0xb3,

    /* U+006D "m" */
    0xde, 0x7b, 0x9e, 0x6c, 0x30, 0xf0, 0xc3, 0xc3,
    0xf, 0xc, 0x3c, 0x30, 0xf0, 0xc3, 0xc3, 0xf,
    0xc, 0x3c, 0x30, 0xc0,

    /* U+006E "n" */
    0xde, 0x71, 0xb0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x60,

    /* U+006F "o" */
    0x1e, 0x1c, 0xe6, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0x61, 0x9c, 0xe1, 0xe0,

    /* U+0070 "p" */
    0xdf, 0x38, 0x6c, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc1, 0xb8, 0xed, 0xe3, 0x0, 0xc0,
    0x30, 0xc, 0x3, 0x0,

    /* U+0071 "q" */
    0x1e, 0xdc, 0x76, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0x60, 0xd8, 0x73, 0xec, 0x3, 0x0,
    0xc0, 0x30, 0xc, 0x3,

    /* U+0072 "r" */
    0xdf, 0x8e, 0x30, 0xc3, 0xc, 0x30, 0xc3, 0xc,
    0x0,

    /* U+0073 "s" */
    0x1f, 0x38, 0x98, 0xc, 0x7, 0xc0, 0xf8, 0x1e,
    0x3, 0x1, 0xb1, 0x8f, 0x80,

    /* U+0074 "t" */
    0x30, 0xc3, 0x3f, 0x30, 0xc3, 0xc, 0x30, 0xc3,
    0xc, 0x30, 0x70,

    /* U+0075 "u" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xb1, 0xdf, 0x60,

    /* U+0076 "v" */
    0xc0, 0xd8, 0x36, 0x9, 0x86, 0x31, 0x8c, 0x41,
    0x30, 0x6c, 0x1a, 0x3, 0x80, 0xc0,

    /* U+0077 "w" */
    0xc3, 0x5, 0x86, 0x1b, 0xe, 0x36, 0x3c, 0x66,
    0x48, 0x8c, 0x93, 0x1b, 0x36, 0x16, 0x6c, 0x28,
    0x50, 0x70, 0xe0, 0xe1, 0xc0,

    /* U+0078 "x" */
    0x61, 0x88, 0x43, 0x30, 0x68, 0x1e, 0x3, 0x1,
    0xe0, 0xc8, 0x33, 0x18, 0x66, 0x18,

    /* U+0079 "y" */
    0xc0, 0xd8, 0x36, 0x8, 0x86, 0x31, 0x8c, 0x41,
    0x30, 0x6c, 0x1a, 0x2, 0x80, 0xe0, 0x10, 0xc,
    0x2, 0x1, 0x81, 0xc0,

    /* U+007A "z" */
    0xff, 0x6, 0x6, 0xc, 0x18, 0x18, 0x30, 0x20,
    0x60, 0xc0, 0xff,

    /* U+007B "{" */
    0x1c, 0xc3, 0xc, 0x30, 0xc3, 0xc, 0x33, 0x3,
    0xc, 0x30, 0xc3, 0xc, 0x30, 0xc1, 0xc0,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf0,

    /* U+007D "}" */
    0xe1, 0x8c, 0x63, 0x18, 0xc6, 0x30, 0x4c, 0x63,
    0x18, 0xc6, 0x31, 0xb8,

    /* U+007E "~" */
    0x70, 0xe4, 0x41, 0xc0,

    /* U+2103 "℃" */
    0x78, 0x1f, 0x21, 0x1c, 0x68, 0x4c, 0xb, 0x33,
    0x0, 0x79, 0x80, 0x0, 0x60, 0x0, 0x18, 0x0,
    0x6, 0x0, 0x1, 0x80, 0x0, 0x60, 0x0, 0x18,
    0x0, 0x3, 0x0, 0x0, 0xc0, 0x0, 0x1c, 0x70,
    0x1, 0xf0
};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 72, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 103, .box_w = 2, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 152, .box_w = 6, .box_h = 5, .ofs_x = 2, .ofs_y = 11},
    {.bitmap_index = 9, .adv_w = 178, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 27, .adv_w = 178, .box_w = 9, .box_h = 19, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 49, .adv_w = 295, .box_w = 17, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 79, .adv_w = 218, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 89, .box_w = 2, .box_h = 5, .ofs_x = 2, .ofs_y = 11},
    {.bitmap_index = 102, .adv_w = 108, .box_w = 5, .box_h = 20, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 115, .adv_w = 108, .box_w = 5, .box_h = 20, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 128, .adv_w = 149, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 10},
    {.bitmap_index = 134, .adv_w = 178, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 147, .adv_w = 89, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 150, .adv_w = 111, .box_w = 5, .box_h = 1, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 151, .adv_w = 89, .box_w = 2, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 152, .adv_w = 125, .box_w = 8, .box_h = 19, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 171, .adv_w = 178, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 178, .box_w = 8, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 178, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 217, .adv_w = 178, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 233, .adv_w = 178, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 251, .adv_w = 178, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 267, .adv_w = 178, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 283, .adv_w = 178, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 299, .adv_w = 178, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 315, .adv_w = 178, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 331, .adv_w = 89, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 334, .adv_w = 89, .box_w = 3, .box_h = 15, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 340, .adv_w = 178, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 352, .adv_w = 178, .box_w = 10, .box_h = 6, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 360, .adv_w = 178, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 372, .adv_w = 152, .box_w = 7, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 385, .adv_w = 303, .box_w = 17, .box_h = 17, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 422, .adv_w = 195, .box_w = 12, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 443, .adv_w = 210, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 461, .adv_w = 204, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 481, .adv_w = 220, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 501, .adv_w = 188, .box_w = 9, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 517, .adv_w = 177, .box_w = 9, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 533, .adv_w = 220, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 553, .adv_w = 233, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 573, .adv_w = 94, .box_w = 2, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 577, .adv_w = 171, .box_w = 8, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 591, .adv_w = 207, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 611, .adv_w = 174, .box_w = 9, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 627, .adv_w = 260, .box_w = 12, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 648, .adv_w = 231, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 668, .adv_w = 237, .box_w = 13, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 691, .adv_w = 203, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 709, .adv_w = 237, .box_w = 13, .box_h = 17, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 737, .adv_w = 203, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 755, .adv_w = 191, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 773, .adv_w = 192, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 793, .adv_w = 231, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 813, .adv_w = 184, .box_w = 12, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 834, .adv_w = 281, .box_w = 16, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 862, .adv_w = 183, .box_w = 11, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 882, .adv_w = 170, .box_w = 10, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 900, .adv_w = 193, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 918, .adv_w = 108, .box_w = 5, .box_h = 19, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 930, .adv_w = 125, .box_w = 8, .box_h = 19, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 949, .adv_w = 108, .box_w = 5, .box_h = 19, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 961, .adv_w = 178, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 972, .adv_w = 179, .box_w = 11, .box_h = 1, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 974, .adv_w = 194, .box_w = 4, .box_h = 4, .ofs_x = 3, .ofs_y = 13},
    {.bitmap_index = 976, .adv_w = 180, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 989, .adv_w = 198, .box_w = 10, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1009, .adv_w = 163, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1022, .adv_w = 198, .box_w = 10, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1042, .adv_w = 177, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1055, .adv_w = 104, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1069, .adv_w = 180, .box_w = 10, .box_h = 16, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1089, .adv_w = 194, .box_w = 9, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1107, .adv_w = 88, .box_w = 2, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1111, .adv_w = 88, .box_w = 4, .box_h = 21, .ofs_x = 0, .ofs_y = -5},
    {.bitmap_index = 1122, .adv_w = 177, .box_w = 9, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1140, .adv_w = 91, .box_w = 3, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1146, .adv_w = 296, .box_w = 14, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1166, .adv_w = 195, .box_w = 9, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1179, .adv_w = 194, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1193, .adv_w = 198, .box_w = 10, .box_h = 16, .ofs_x = 2, .ofs_y = -5},
    {.bitmap_index = 1213, .adv_w = 198, .box_w = 10, .box_h = 16, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1233, .adv_w = 124, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1242, .adv_w = 150, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1255, .adv_w = 121, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1266, .adv_w = 194, .box_w = 9, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1279, .adv_w = 167, .box_w = 10, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1293, .adv_w = 257, .box_w = 15, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1314, .adv_w = 159, .box_w = 10, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1328, .adv_w = 167, .box_w = 10, .box_h = 16, .ofs_x = 0, .ofs_y = -5},
    {.bitmap_index = 1348, .adv_w = 152, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1359, .adv_w = 108, .box_w = 6, .box_h = 19, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1374, .adv_w = 86, .box_w = 2, .box_h = 22, .ofs_x = 2, .ofs_y = -6},
    {.bitmap_index = 1380, .adv_w = 108, .box_w = 5, .box_h = 19, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1392, .adv_w = 178, .box_w = 9, .box_h = 3, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 1396, .adv_w = 320, .box_w = 18, .box_h = 15, .ofs_x = 1, .ofs_y = 0}
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
    },
    {
        .range_start = 8451, .range_length = 1, .glyph_id_start = 96,
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
    42, 43, 44, 45, 2, 0, 0, 0,
    0
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
    35, 36, 37, 38, 0, 0, 3, 0,
    0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, -41, 0, -41, 0,
    0, 0, 0, -20, 0, -34, -4, 0,
    0, 0, 0, -4, 0, 0, 0, 0,
    -11, 0, 0, 0, 0, 0, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 28, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -34, 0, -49,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -35, -7, -23, -12, 0,
    -33, 0, 0, 0, -4, 0, 0, 0,
    9, 0, 0, -16, 0, -12, -8, 0,
    -7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -7,
    -6, -17, 0, -7, -4, -10, -24, -7,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -9, 0, -3, 0, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -14, -4, -28, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -9,
    -11, 0, -4, 9, 9, 0, 0, 3,
    -7, 0, 0, 0, 0, 0, 0, 0,
    0, -18, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -9, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -20, 0, -34,
    0, 0, 0, 0, 0, 0, -9, -2,
    -4, 0, 0, -20, -5, -5, 0, 1,
    -5, -3, -14, 8, 0, -4, 0, 0,
    0, 0, 8, -5, -2, -3, -1, -1,
    -3, 0, 0, 0, 0, -11, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    -5, -9, 0, -2, -1, -1, -5, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, -5, -4, -4, -5, 0,
    0, 0, 0, 0, 0, -9, 0, 0,
    0, 0, 0, 0, -10, -4, -9, -6,
    -5, -1, -1, -1, -3, -4, 0, 0,
    0, 0, -7, 0, 0, 0, 0, -10,
    -4, -5, -4, 0, -5, 0, 0, 0,
    0, -12, 0, 0, 0, -6, 0, 0,
    0, -4, 0, -14, 0, -9, 0, -4,
    -2, -6, -7, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, 0, 0, -9, 0, -4, 0, -11,
    -4, 0, 0, 0, 0, 0, -26, 0,
    -26, -25, 0, 0, 0, -13, -4, -50,
    -7, 0, 0, 1, 1, -9, 0, -11,
    0, -12, -5, 0, -9, 0, 0, -7,
    -7, -4, -6, -7, -6, -9, -6, -11,
    0, 0, 0, -10, 0, 0, 0, 0,
    0, 0, 0, -1, 0, 0, 0, -7,
    0, -5, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -9, 0, -9, 0, 0, 0,
    0, 0, 0, -14, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -7, 0, -14,
    0, -11, 0, 0, 0, 0, -3, -4,
    -7, 0, -3, -6, -5, -4, -4, 0,
    -6, 0, 0, 0, -3, 0, 0, 0,
    -4, 0, 0, -12, -5, -7, -6, -6,
    -7, -5, 0, -32, 0, -54, 0, -19,
    0, 0, 0, 0, -12, 1, -9, 0,
    -8, -43, -10, -27, -20, 0, -27, 0,
    -28, 0, -4, -5, -1, 0, 0, 0,
    0, -7, -4, -13, -12, 0, -13, 0,
    0, 0, 0, 0, -40, -12, -40, -27,
    0, 0, 0, -18, 0, -52, -4, -9,
    0, 0, 0, -9, -4, -28, 0, -15,
    -9, 0, -11, 0, 0, 0, -4, 0,
    0, 0, 0, -5, 0, -7, 0, 0,
    0, -4, 0, -11, 0, 0, 0, 0,
    0, -1, 0, -6, -5, -5, 0, 2,
    2, -1, 0, -4, 0, -1, -4, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -3, 0, 0, 0, -6,
    0, 5, 0, 0, 0, 0, 0, 0,
    0, -5, -5, -7, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -9, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -38, -26,
    -38, -32, -7, -7, 0, -14, -9, -45,
    -14, 0, 0, 0, 0, -7, -5, -19,
    0, -26, -24, -6, -26, 0, 0, -17,
    -21, -6, -17, -12, -12, -14, -12, -27,
    0, 0, 0, 0, -6, 0, -6, -11,
    0, 0, 0, -6, 0, -17, -4, 0,
    0, -1, 0, -4, -5, 0, 0, -1,
    0, 0, -4, 0, 0, 0, -1, 0,
    0, 0, 0, -3, 0, 0, 0, 0,
    0, 0, -23, -7, -23, -17, 0, 0,
    0, -5, -4, -26, -4, 0, -4, 4,
    0, 0, 0, -7, 0, -8, -5, 0,
    -8, 0, 0, -7, -4, 0, -11, -3,
    -3, -5, -3, -9, 0, 0, 0, 0,
    -12, -4, -12, -11, 0, 0, 0, 0,
    -2, -23, -2, 0, 0, 0, 0, 0,
    0, -2, 0, -6, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, -4, 0, -4, 0, -10,
    0, 0, 0, 0, 0, 1, -6, 0,
    -5, -7, -4, 0, 0, 0, 0, 0,
    0, -4, -3, -6, 0, 0, 0, 0,
    0, -6, -4, -6, -5, -4, -6, -5,
    0, 0, 0, 0, -32, -24, -32, -24,
    -9, -9, -3, -5, -5, -36, -6, -5,
    -4, 0, 0, 0, 0, -9, 0, -24,
    -15, 0, -21, 0, 0, -14, -15, -10,
    -12, -5, -9, -12, -5, -17, 0, 0,
    0, 0, 0, -12, 0, 0, 0, 0,
    0, -2, -7, -12, -11, 0, -4, -2,
    -2, 0, -5, -6, 0, -6, -8, -7,
    -5, 0, 0, 0, 0, -5, -8, -6,
    -6, -9, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -31, -11, -19, -11, 0,
    -26, 0, 0, 0, 0, 0, 12, 0,
    26, 0, 0, 0, 0, -7, -4, 0,
    4, 0, 0, 0, 0, -19, 0, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, -9, 0, -6, -1, 0, -9, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, -11, 0, -9, -4, 2, -4, 0,
    0, 0, -4, 0, 0, 0, 0, -20,
    0, -7, 0, -1, -17, 0, -9, -5,
    0, 0, 0, 0, 0, 0, 0, -6,
    0, -1, -1, -6, -1, -2, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -7, 0, -5,
    0, 0, -9, 0, 0, -4, -8, 0,
    -4, 0, 0, 0, 0, -4, 0, 2,
    2, 3, 2, 0, 0, 0, 0, -12,
    0, 4, 0, 0, 0, 0, -3, 0,
    0, -7, -7, -9, 0, -6, -4, 0,
    -9, 0, -7, -5, 0, 0, -4, 0,
    0, 0, 0, -4, 0, 2, 2, -3,
    2, 0, 5, 14, 18, 0, -18, -5,
    -18, -5, 0, 0, 9, 0, 0, 0,
    0, 17, 0, 24, 17, 12, 21, 0,
    22, -7, -4, 0, -5, 0, -4, 0,
    -1, 0, 0, 4, 0, -1, 0, -5,
    0, 0, 5, -12, 0, 0, 0, 17,
    0, 0, -13, 0, 0, 0, 0, -9,
    0, 0, 0, 0, -5, 0, 0, -6,
    -5, 0, 0, 0, 13, 0, 0, 0,
    0, -1, -1, 0, 5, -5, 0, 0,
    0, -12, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, -9, 0, -4,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -8,
    4, -16, 4, 0, 4, 4, -4, 0,
    0, 0, 0, -12, 0, 0, 0, 0,
    -4, 0, 0, -4, -6, 0, -4, 0,
    -4, 0, 0, -7, -5, 0, 0, -3,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -9, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -7,
    0, -5, 0, 0, -11, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -20, -9, -20, -12, 9, 9,
    0, -5, 0, -20, 0, 0, 0, 0,
    0, 0, 0, -4, 4, -9, -4, 0,
    -4, 0, 0, 0, -1, 0, 0, 9,
    7, 0, 9, -1, 0, 0, 0, -18,
    0, 4, 0, 0, 0, 0, -4, 0,
    0, 0, 0, -9, 0, -4, 0, 0,
    -7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, -9,
    2, 4, 5, 5, -9, 0, 0, 0,
    0, -5, 0, 0, 0, 0, -1, 0,
    0, -8, -5, 0, -4, 0, 0, 0,
    -4, -7, 0, 0, 0, -6, 0, 0,
    0, 0, 0, -4, -12, -3, -12, -7,
    0, 0, 0, -4, 0, -14, 0, -7,
    0, -3, 0, 0, -5, -4, 0, -7,
    -1, 0, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, -9, 0, 0,
    0, -4, -14, 0, -14, -3, 0, 0,
    0, -1, 0, -11, 0, -9, 0, -3,
    0, -5, -9, 0, 0, -4, -1, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, -6, -5, 0, 0, -8,
    3, -5, -3, 0, 0, 3, 0, 0,
    -4, 0, -1, -12, 0, -5, 0, -4,
    -12, 0, 0, -4, -6, 0, 0, 0,
    0, 0, 0, -9, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -12, 0,
    -12, -5, 0, 0, 0, 0, 0, -14,
    0, -7, 0, -1, 0, -1, -3, 0,
    0, -7, -1, 0, 0, 0, -4, 0,
    0, 0, 0, 0, 0, -5, 0, -9,
    0, 0, 0, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -9,
    0, 0, 0, 0, -11, 0, 0, -8,
    -4, 0, -2, 0, 0, 0, 0, 0,
    -4, -1, 0, 0, -1, 0
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
    .cmap_num = 2,
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
const lv_font_t ui_font_siyuanheiti_20 = {
#else
lv_font_t ui_font_siyuanheiti_20 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 23,          /*The maximum line height required by the font*/
    .base_line = 6,             /*Baseline measured from the bottom of the line*/
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

#endif /*#if UI_FONT_SIYUANHEITI_20*/