#ifndef __SH1106_FONTS_H__
#define __SH1106_FONTS_H__

#include "sh1106.h"

#ifdef SH1106_INCLUDE_FONT_6x8
extern const SH1106_Font_t Font_6x8;
#endif
#ifdef SH1106_INCLUDE_FONT_7x10
extern const SH1106_Font_t Font_7x10;
#endif
#ifdef SH1106_INCLUDE_FONT_11x18
extern const SH1106_Font_t Font_11x18;
#endif
#ifdef SH1106_INCLUDE_FONT_16x26
extern const SH1106_Font_t Font_16x26;
#endif
#ifdef SH1106_INCLUDE_FONT_16x24
extern const SH1106_Font_t Font_16x24;
#endif
#ifdef SH1106_INCLUDE_FONT_16x15
/** Generated Roboto Thin 15 
 * @copyright Google https://github.com/googlefonts/roboto
 * @license This font is licensed under the Apache License, Version 2.0.
*/
extern const SH1106_Font_t Font_16x15;
#endif

#endif // __SH1106_FONTS_H__
