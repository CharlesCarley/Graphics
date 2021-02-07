/*
-------------------------------------------------------------------------------
    Copyright (c) Charles Carley.

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#ifndef _skGraphics_h_
#define _skGraphics_h_

#include "Graphics/skGraphicsConfig.h"
#include "Image/skImageTypes.h"
#include "Math/skScalar.h"

SK_ST_C;

#define SK_NO_STATUS (-1)

#define SK_DEFAULT_VERTICES_PER_SEGMENT 16
#define SK_MIN_VERTICES_PER_SEGMENT 3
#define SK_MAX_VERTICES_PER_SEGMENT 128
#define SK_DEFAULT_PROJECTION_MODE SK_STANDARD
#define SK_DEFAULT_METRICS_MODE SK_PIXEL
#define SK_MIN_DPI 24
#define SK_MAX_DPI 300
#define SK_MIN_FONT_SIZE 8
#define SK_MAX_FONT_SIZE 96


#define SK_SIZE_HANDLE(x) \
    typedef struct x##_t  \
    {                     \
        size_t unused;    \
    } * x

SK_SIZE_HANDLE(SKcontext);
SK_SIZE_HANDLE(SKpaint);
SK_SIZE_HANDLE(SKpath);
SK_SIZE_HANDLE(SKimage);
SK_SIZE_HANDLE(SKfont);
SK_SIZE_HANDLE(SKcachedString);

enum SKbackend
{
    SK_BE_None,
    SK_BE_OpenGL,
};

typedef skScalar SKscalar;

typedef struct SKrecti
{
    SKint32 x;
    SKint32 y;
    SKint32 w;
    SKint32 h;
} SKrecti;

typedef struct SKrectf
{
    SKscalar x, y, w, h;
} SKrectf;

typedef struct SKaabbf
{
    SKscalar x1, y1;
    SKscalar x2, y2;
} SKaabbf;

typedef struct SKvec2i
{
    SKint32 x, y;
} SKvec2i;

typedef struct SKvec2f
{
    SKscalar x, y;
} SKvec2f;

typedef struct SKcolor4s
{
    SKscalar r, g, b, a;
} SKcolor4s;

typedef struct SKcolor4b
{
    SKubyte r, g, b, a;
} SKcolor4b;

typedef SKuint32 SKcolori;

typedef struct SKcolorStop
{
    SKscalar offset;
    SKcolori color;
} SKcolorStop;

typedef SKint32 SKenum;

typedef enum SKgradientType
{
    SK_LINEAR,
    SK_RADIAL,
} SKgradientType;

enum SKPenStyle
{
    SK_PS_MIN,
    SK_PS_SOLID,
    SK_PS_DASHED,
    SK_PS_MAX,
};
typedef SKenum SKpenStyle;

enum SKBrushStyle
{
    SK_BS_MIN,
    SK_BS_SOLID,
    SK_BS_PATTERN,
    SK_BS_MAX,
};

typedef SKenum SKbrushStyle;

enum SKBrushMode
{
    SK_BM_MIN,
    SK_BM_REPLACE,
    SK_BM_ADD,
    SK_BM_MODULATE,
    SK_BM_SUBTRACT,
    SK_BM_DIVIDE,
    SK_BM_MAX,
};
typedef SKenum SKbrushMode;

enum SKPaintStyle
{
    SK_PEN_STYLE,
    SK_BRUSH_STYLE,
    SK_BRUSH_MODE,
    SK_PEN_WIDTH,
    SK_BRUSH_COLOR,
    SK_PEN_COLOR,
    SK_SURFACE_COLOR,
    SK_BRUSH_PATTERN,
    SK_LINE_TYPE,
    SK_AUTO_CLEAR,
};

typedef SKenum SKpaintStyle;

enum SKFiltering
{
    SK_FILTER_MIN,
    SK_FILTER_NONE,
    SK_FILTER_NONE_LINEAR,
    SK_FILTER_LINEAR_NONE,
    SK_FILTER_BI_LINEAR,
    SK_FILTER_MAX,
};

typedef SKenum SKfiltering;
typedef SKenum SKpixelFormat;

enum SKContextOptionEnum
{
    SK_VERTICES_PER_SEGMENT,
    SK_CLEAR_COLOR,
    SK_CLEAR_RECT,
    SK_CONTEXT_SIZE,
    SK_CONTEXT_SCALE,
    SK_CONTEXT_BIAS,
    SK_OPACITY,
    SK_METRICS_MODE,
    SK_USE_CURRENT_VIEWPORT,
    SK_PROJECTION_TYPE,
    SK_Y_UP,
};

typedef SKenum SKcontextOptionEnum;

enum SKProjectionType
{
    SK_CARTESIAN,
    SK_STANDARD,
};

typedef SKenum SKprojectionType;

enum SKMetricsMode
{
    SK_PIXEL,
    SK_RELATIVE,
};
typedef SKenum SKmetricsMode;

enum SKLineType
{
    SK_LT_MIN,
    SK_LINE_LOOP,
    SK_LINE_LIST,
    SK_POINTS,
    SK_LT_MAX,
};
typedef SKenum SKlineType;

enum SKCorner
{
    SK_CNR_NONE = 0,
    SK_CNR_LT   = 0x01,
    SK_CNR_RT   = 0x02,
    SK_CNR_RB   = 0x04,
    SK_CNR_LB   = 0x08,
    SK_CNR_ALL  = SK_CNR_LT | SK_CNR_RT | SK_CNR_RB | SK_CNR_LB
};
typedef SKenum SKcorner;

enum SKDirection
{
    SK_NORTH = 0x01,
    SK_EAST  = 0x02,
    SK_SOUTH = 0x04,
    SK_WEST  = 0x08,
};
typedef SKenum SKdirection;

enum SKWinding
{
    SK_CW,
    SK_CCW,
};
typedef SKenum SKwinding;

typedef struct SKtextExtent
{
    SKscalar width, height;
    SKscalar ascent, descent;
    SKscalar externalLeading;
} SKtextExtent;

enum SKFontOptionEnum
{
    SK_FONT_FILTER,
    SK_FONT_MIPMAP,
    SK_FONT_SIZE,
    SK_FONT_DPI,
    SK_FONT_TAB_SIZE,
};
typedef SKenum SKfontOptionEnum;

enum SKBuiltinFont
{
    SK_FONT_DEFAULT,
    SK_FONT_DEFAULT_FIXED,
#ifdef Graphics_EXTRA_BUILTIN_FONTS
    // https://fonts.google.com/
    // Rock_Salt
    SK_FONT_SPC1,
    // Caveat
    SK_FONT_SPC2,
    // Roboto
    SK_FONT_UI,
    SK_FONT_UI_LIGHT,
#endif
    SK_FONT_MAX,
};
typedef SKenum SKbuiltinFont;

enum SKImageOptionEnum
{
    SK_IMAGE_FILTER,
    SK_IMAGE_MIPMAP,
    SK_IMAGE_WIDTH,
    SK_IMAGE_HEIGHT,
    SK_IMAGE_PITCH,
    SK_IMAGE_BPP,
    SK_IMAGE_SIZE_IN_BYTES,
    SK_IMAGE_BYTES,
    SK_IMAGE_PIXEL_FORMAT,
};
typedef SKenum SKimageOptionEnum;

enum SKStringOptionEnum
{
    SK_STRING_SIZE,
    SK_STRING_EXTENT,
};
typedef SKenum SKstringOptionEnum;

SK_API SKcontext skNewContext();
SK_API SKcontext skNewBackEndContext(SKenum backend);
SK_API void      skDeleteContext(SKcontext ctx);
SK_API void      skSetCurrentContext(SKcontext ctx);
SK_API SKcontext skGetCurrentContext();
SK_API void      skClearContext();
SK_API void      skClear(SKscalar x, SKscalar y, SKscalar w, SKscalar h);

SK_API void skSetContext1i(SKcontextOptionEnum en, SKint32 v);
SK_API void skSetContext1f(SKcontextOptionEnum en, SKscalar v);

SK_API void skGetContext1i(SKcontextOptionEnum en, SKint32* v);
SK_API void skGetContext1f(SKcontextOptionEnum en, SKscalar* v);

SK_API void skGetContext2i(SKcontextOptionEnum en, SKint32* v);
SK_API void skGetContext2f(SKcontextOptionEnum en, SKscalar* v);

SK_API void skSetContext2i(SKcontextOptionEnum en, SKint32 i0, SKint32 i1);
SK_API void skSetContext2f(SKcontextOptionEnum en, SKscalar f0, SKscalar f1);

SK_API void skSetContext4i(SKcontextOptionEnum en, SKint32 i0, SKint32 i1, SKint32 i2, SKint32 i3);
SK_API void skSetContext4f(SKcontextOptionEnum en, SKscalar f0, SKscalar f1, SKscalar f2, SKscalar f3);

SK_API void skClearColor4f(SKscalar r, SKscalar g, SKscalar b, SKscalar a);
SK_API void skClearColor1i(SKcolori rgba);

SK_API SKpaint skGetWorkingPaint();
SK_API SKpath  skGetWorkingPath();

/**********************************************************
    Transforms
*/
SK_API void skLoadIdentity();
SK_API void skTranslate(SKscalar x, SKscalar y);
SK_API void skScale(SKscalar x, SKscalar y);
SK_API void skRotate(SKscalar r);

/**********************************************************
    View projection
*/

SK_API void skProjectContext(SKprojectionType pt);
SK_API void skProjectRect(SKscalar x, SKscalar y, SKscalar w, SKscalar h);
SK_API void skProjectBox(SKscalar x1, SKscalar y1, SKscalar x2, SKscalar y2);

/**********************************************************
    Paint objects
*/

SK_API SKpaint skNewPaint();
SK_API void    skDeletePaint(SKpaint obj);
SK_API void    skSelectPaint(SKpaint obj);

SK_API void skColor1ui(SKuint32 c);
SK_API void skColor3f(SKscalar r, SKscalar g, SKscalar b);
SK_API void skColor4f(SKscalar r, SKscalar g, SKscalar b, SKscalar a);
SK_API void skColor3ub(SKubyte r, SKubyte g, SKubyte b);
SK_API void skColor4ub(SKubyte r, SKubyte g, SKubyte b, SKubyte a);

SK_API void skSetPaint1i(SKpaintStyle en, SKint32 v);
SK_API void skSetPaint1f(SKpaintStyle en, SKscalar v);
SK_API void skSetPaint1ui(SKpaintStyle en, SKuint32 c);

SK_API void skGetPaint1i(SKpaintStyle en, SKint32* v);
SK_API void skGetPaint1f(SKpaintStyle en, SKscalar* v);
SK_API void skGetPaint1ui(SKpaintStyle en, SKuint32* v);

/**********************************************************
   Images
*/

SK_API SKimage skNewImage();
SK_API void    skDeleteImage(SKimage ima);
SK_API void    skSelectImage(SKimage ima);

SK_API SKimage skCreateImage(SKuint32 w, SKuint32 h, SKint32 format);

SK_API void skImageLinearGradient(SKimage      ima,
                                  SKcolorStop* stops,
                                  SKint32      stopCount,
                                  SKuint16     dir);
SK_API void skImageRadialGradient(SKimage      ima,
                                  SKcolorStop* stops,
                                  SKint32      stopCount);

SK_API void skImageLinearGradientEx(SKimage      ima,
                                    SKint32      fx,
                                    SKint32      fy,
                                    SKint32      tx,
                                    SKint32      ty,
                                    SKcolorStop* stops,
                                    SKint32      stopCount);

SK_API void skImageRadialGradientEx(SKimage      ima,
                                    SKint32      fx,
                                    SKint32      fy,
                                    SKint32      tx,
                                    SKint32      ty,
                                    SKint32      rx,
                                    SKint32      ry,
                                    SKcolorStop* stops,
                                    SKint32      stopCount);

SK_API void    skImageSave(SKimage ima, const char* path);
SK_API SKimage skImageLoad(const char* path);
SK_API void    skSetImageUV(SKscalar x, SKscalar y, SKscalar w, SKscalar h);

SK_API void skSetImage1i(SKimage image, SKimageOptionEnum en, SKint32 v);
SK_API void skGetImage1i(SKimage image, SKimageOptionEnum en, SKint32* v);
SK_API void skSetImage1f(SKimage image, SKimageOptionEnum en, SKscalar v);
SK_API void skGetImage1f(SKimage image, SKimageOptionEnum en, SKscalar* v);

/**********************************************************
   Fonts (TTF)
*/

SK_API SKfont skNewFont(SKbuiltinFont font, SKuint32 size, SKuint32 dpi);
SK_API void   skSelectFont(SKfont font);
SK_API void   skDeleteFont(SKfont font);
SK_API SKfont skNewFontFromFile(const char* path, SKuint32 size, SKuint32 dpi);

SK_API SKint32 skGetFontAverageWidth(SKfont font);
SK_API void    skGetFontTextExtentEx(SKfont font, const char* text, SKint32 chidx, SKint32 len, SKint32* w, SKint32* h);
SK_API void    skGetFontTextExtent(SKfont font, const char* text, SKint32 len, SKint32* w, SKint32* h);
SK_API void    skGetFontCharExtent(SKfont font, char ch, SKint32* w, SKint32* h);
SK_API void    skGetFontCharExtentEx(SKfont font, char ch, SKtextExtent* te);

SK_API void skSetFont1i(SKfont font, SKfontOptionEnum en, SKint32 v);
SK_API void skGetFont1i(SKfont font, SKfontOptionEnum en, SKint32* v);
SK_API void skSetFont1f(SKfont font, SKfontOptionEnum en, SKscalar v);
SK_API void skGetFont1f(SKfont font, SKfontOptionEnum en, SKscalar* v);

/**********************************************************
   Paths
*/

SK_API SKpath skNewPath();
SK_API void   skDeletePath(SKpath pth);
SK_API void   skSelectPath(SKpath pth);

SK_API void skPathSetScale(SKscalar sx, SKscalar sy);
SK_API void skPathSetBias(SKscalar tx, SKscalar ty);

SK_API void skPathTransform(SKscalar* matrix);
SK_API void skGetPathBoundingBox(SKaabbf* bb);

SK_API void skMoveTo(SKscalar x, SKscalar y);
SK_API void skLineTo(SKscalar x, SKscalar y);
SK_API void skPutVert(SKscalar x, SKscalar y, SKuint8 move);

SK_API void skCubicTo(SKscalar fx, SKscalar fy, SKscalar tx, SKscalar ty);
SK_API void skRectTo(SKscalar fx, SKscalar fy, SKscalar tx, SKscalar ty);

SK_API void skArcTo(SKscalar x1, SKscalar y1, SKscalar x2, SKscalar y2, SKscalar angle1, SKscalar angle2, SKwinding winding);
SK_API void skArc(SKscalar x, SKscalar y, SKscalar radius, SKscalar angle1, SKscalar angle2, SKwinding winding);
SK_API void skClosePath();
SK_API void skClearPath();

/**********************************************************
   Common Polygons
*/

SK_API void skRect(SKscalar x, SKscalar y, SKscalar w, SKscalar h);
SK_API void skLine(SKscalar x1, SKscalar y1, SKscalar x2, SKscalar y2);
SK_API void skEllipse(SKscalar x, SKscalar y, SKscalar w, SKscalar h);
SK_API void skRoundRect(SKscalar x, SKscalar y, SKscalar w, SKscalar h, SKscalar aw, SKscalar ah, SKuint16 corners);
SK_API void skStar(SKscalar x, SKscalar y, SKscalar w, SKscalar h, SKint32 Q, SKint32 P);
SK_API void skPolygon(SKscalar* vertices, SKuint32 count, SKuint32 close_);

/**********************************************************
   Fills
*/

SK_API void skFill();
SK_API void skStroke();

/**********************************************************
   Strings
*/

SK_API SKcachedString skNewCachedString();
SK_API void           skDeleteCachedString(SKcachedString str);
SK_API void           skDisplayCachedString(SKcachedString str);
SK_API void           skRebuildCachedString(SKcachedString str);
SK_API void           skBuildCachedString(SKcachedString str, const char* data);
SK_API void           skGetCachedString2fv(const SKcachedString str, SKstringOptionEnum en, SKscalar* v);
SK_API void           skDisplayString(SKfont font, const char* str, SKint32 len, SKscalar x, SKscalar y);
SK_API void           skDisplayFormattedString(SKfont font, SKscalar x, SKscalar y, const char* str, ...);

#ifndef Graphics_NO_PALETTE

const SKuint32 CS_Grey00           = 0x000000FF;
const SKuint32 CS_Grey01           = 0x191919FF;
const SKuint32 CS_Grey02           = 0x222222FF;
const SKuint32 CS_Grey03           = 0x333333FF;
const SKuint32 CS_Grey04           = 0x4C4C4CFF;
const SKuint32 CS_Grey05           = 0x666666FF;
const SKuint32 CS_Grey06           = 0x7F7F7FFF;
const SKuint32 CS_Grey07           = 0x999999FF;
const SKuint32 CS_Grey08           = 0xB2B2B2FF;
const SKuint32 CS_Grey09           = 0xCCCCCCFF;
const SKuint32 CS_Grey10           = 0xFFFFFFFF;
const SKuint32 CS_Grey04HL         = 0x494949FF;
const SKuint32 CS_Color01          = 0x9C6855FF;
const SKuint32 CS_Color01HL        = 0xE5CCC3FF;
const SKuint32 CS_Color02          = 0xAB9E56FF;
const SKuint32 CS_Color02HL        = 0xE5DDACFF;
const SKuint32 CS_Color03          = 0x9E9D3CFF;
const SKuint32 CS_Color03HL        = 0xE5E5ACFF;
const SKuint32 CS_Color04          = 0x56799EFF;
const SKuint32 CS_Color04HL        = 0xC3D4E5FF;
const SKuint32 CS_Color05          = 0x375E8EFF;
const SKuint32 CS_Color05HL        = 0xC3D3E5FF;
const SKuint32 CS_Color06          = 0x435561FF;
const SKuint32 CS_Color06HL        = 0xC3D8E5FF;
const SKuint32 CS_Color07          = 0x567F9EFF;
const SKuint32 CS_Color07HL        = 0xB8D2E5FF;
const SKuint32 CS_Color08          = 0x4885AFFF;
const SKuint32 CS_Color08HL        = 0xC3D7E5FF;
const SKuint32 CS_Color09          = 0x836955FF;
const SKuint32 CS_Color09HL        = 0xE5C5ACFF;
const SKuint32 CS_Color10          = 0xC18B63FF;
const SKuint32 CS_Color10HL        = 0xE5C4ACFF;
const SKuint32 CS_Color11          = 0x865FC5FF;
const SKuint32 CS_Color11HL        = 0xB18CECFF;
const SKuint32 CS_Indicator        = 0x000000FF;
const SKuint32 CS_Green            = 0x99CC00FF;
const SKuint32 CS_BaseRed          = 0xC00000FF;
const SKuint32 CS_BaseGreen        = 0x92D050FF;
const SKuint32 CS_BaseRedHL        = 0xFF1C1CFF;
const SKuint32 CS_BaseGreenHL      = 0xAAF25DFF;
const SKuint32 CS_Color081         = 0x528FB9FF;
const SKuint32 CS_Color082         = 0x66A3CDFF;
const SKuint32 CS_Color083         = 0x7AB7E1FF;
const SKuint32 CS_Color084         = 0x8ECBF5FF;
const SKuint32 CS_Color085         = 0xA2DF109F;
const SKuint32 CS_Macab031         = 0xC7BBAFFF;
const SKuint32 CS_Macab032         = 0xD1C5B9FF;
const SKuint32 CS_Macab033         = 0xDBCFC3FF;
const SKuint32 CS_Macab034         = 0xE5D9CDFF;
const SKuint32 CS_Macab035         = 0xEFE3D7FF;
const SKuint32 CS_Macab051         = 0xDDBFFFFF;
const SKuint32 CS_Macab052         = 0x171715FF;
const SKuint32 CS_Macab053         = 0x21211FFF;
const SKuint32 CS_Macab054         = 0x2B2B29FF;
const SKuint32 CS_Macab055         = 0x353533FF;
const SKuint32 CS_LightBackground  = 0xFFFFFFFF;
const SKuint32 CS_LabelEmphasis3   = 0x123551FF;
const SKuint32 CS_LabelEmphasis2   = 0x617A8EFF;
const SKuint32 CS_LabelEmphasis1   = 0xBEC9D2FF;
const SKuint32 CS_LabelEmphasisA3  = 0x056CB7FF;
const SKuint32 CS_LabelEmphasisA2  = 0x5BA0D3FF;
const SKuint32 CS_LabelEmphasisA1  = 0xB9DAF2FF;
const SKuint32 CS_ButtonText       = 0x556069FF;
const SKuint32 CS_Tiny01           = 0x958DBDFF;
const SKuint32 CS_Tiny02           = 0x242B3EFF;
const SKuint32 CS_Tiny03           = 0x8D4141FF;
const SKuint32 CS_Tiny04           = 0xF1F0F0FF;
const SKuint32 CS_Tiny05           = 0x100C0CFF;
const SKuint32 CS_Macab01          = 0x3F1016FF;
const SKuint32 CS_Macab02          = 0x595854FF;
const SKuint32 CS_Macab03          = 0xBDB1A5FF;
const SKuint32 CS_Macab04          = 0x8C413FFF;
const SKuint32 CS_Macab05          = 0x0D0D0BFF;
const SKuint32 CS_Pismo01          = 0x055A8FFF;
const SKuint32 CS_Pismo02          = 0x04648EFF;
const SKuint32 CS_Pismo03          = 0x5EC4F6FF;
const SKuint32 CS_Pismo04          = 0x197276FF;
const SKuint32 CS_Pismo05          = 0x99F2FFFF;
const SKuint32 CS_OBContrast01     = 0x7B9CA4FF;
const SKuint32 CS_OBContrast02     = 0x51584CFF;
const SKuint32 CS_OBContrast03     = 0xD7A260FF;
const SKuint32 CS_OBContrast04     = 0x3D2115FF;
const SKuint32 CS_OBContrast05     = 0x100C0DFF;
const SKuint32 CS_OffWhite         = 0xF0F0F0FF;
const SKuint32 CS_OffWhiteM        = 0xF5F5F5FF;
const SKuint32 CS_OffWhiteD0       = 0xFAFAFAFF;
const SKuint32 CS_OffWhiteD1       = 0xEDEDEDFF;
const SKuint32 CS_OffWhiteD2       = 0xE3E3E3FF;
const SKuint32 CS_OffWhiteD3       = 0xD9D9D9FF;
const SKuint32 CS_Transparent      = 0xFFFFFFFF;
const SKuint32 CS_Glass            = 0xFAFAFAFF;
const SKuint32 CS_Macab011         = 0x481A20FF;
const SKuint32 CS_Macab012         = 0x52242AFF;
const SKuint32 CS_Macab013         = 0x5C2E34FF;
const SKuint32 CS_Macab014         = 0x66383EFF;
const SKuint32 CS_Macab015         = 0x704248FF;
const SKuint32 CS_Macab021         = 0x63625EFF;
const SKuint32 CS_Macab022         = 0x6D6C68FF;
const SKuint32 CS_Macab023         = 0x777672FF;
const SKuint32 CS_Macab024         = 0x81807CFF;
const SKuint32 CS_Macab025         = 0x8B8A86FF;
const SKuint32 CS_Macab041         = 0x964B49FF;
const SKuint32 CS_Macab042         = 0xA05553FF;
const SKuint32 CS_Macab043         = 0xAA5F5DFF;
const SKuint32 CS_Macab044         = 0xB46967FF;
const SKuint32 CS_Macab045         = 0xBE7371FF;
const SKuint32 CS_DarkBackground   = 0x333333FF;
const SKuint32 CS_LabelText        = 0xA5A5A5FF;
const SKuint32 CS_LabelAccent      = 0x538AB2FF;
const SKuint32 CS_ButtonBackground = 0x555555FF;

#endif  // !SK_NO_PALLETTE

SK_EN_C;

#endif /*_skGraphics_h_*/
