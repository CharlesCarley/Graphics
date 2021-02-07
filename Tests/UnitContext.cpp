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
#include "Catch2.h"
#include "Graphics/skGraphics.h"
#include "Utils/skDisableWarnings.h"

bool feq(float a, float b)
{
    return fabs(a - b) < FLT_EPSILON;
}

void AssertEqualI(SKint32 option, const SKint32 expected)
{
    SKint32 prop;
    prop = SK_NO_STATUS;
    skGetContext1i(option, &prop);
    EXPECT_EQ(expected, prop);
}

void AssertEqualF(SKint32 option, const SKscalar expected)
{
    SKscalar prop;
    prop = SK_NO_STATUS;
    skGetContext1f(option, &prop);
    EXPECT_TRUE(feq(expected, prop));
}

void AssertPaintEqualI(SKint32 option, const SKint32 expected)
{
    SKint32 prop;
    prop = SK_NO_STATUS;
    skGetPaint1i(option, &prop);
    EXPECT_EQ(expected, prop);
}

void AssertPaintEqualUI(SKint32 option, const SKuint32 expected)
{
    SKuint32 prop;
    prop = SK_NO_STATUS;
    skGetPaint1ui(option, &prop);
    EXPECT_EQ(expected, prop);
}

void AssertPaintEqualF(SKint32 option, const SKscalar expected)
{
    SKscalar prop;
    prop = SK_NO_STATUS;
    skGetPaint1f(option, &prop);
    EXPECT_TRUE(feq(expected, prop));
}

TEST_CASE("ContextCreate")
{
    SKcontext ctx1 = skNewBackEndContext(SK_BE_None);

    EXPECT_NE(ctx1, nullptr);
    EXPECT_EQ(ctx1, skGetCurrentContext());

    SKcontext ctx2 = skNewBackEndContext(SK_BE_None);

    EXPECT_NE(ctx2, nullptr);
    EXPECT_EQ(ctx2, skGetCurrentContext());

    skSetCurrentContext(ctx1);
    EXPECT_NE(ctx2, skGetCurrentContext());
    EXPECT_EQ(ctx1, skGetCurrentContext());

    skDeleteContext(ctx1);
    EXPECT_EQ(skGetCurrentContext(), nullptr);
    skDeleteContext(ctx2);
}

TEST_CASE("ContextGetDefaults")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    AssertEqualI(SK_USE_CURRENT_VIEWPORT, 0);
    AssertEqualI(SK_METRICS_MODE, SK_PIXEL);
    AssertEqualI(SK_Y_UP, 0);
    AssertEqualI(SK_PROJECTION_TYPE, SK_STANDARD);

    skDeleteContext(ctx);
}

TEST_CASE("SK_PROJECTION_TYPE")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    AssertEqualI(SK_PROJECTION_TYPE, SK_STANDARD);
    AssertEqualI(SK_Y_UP, 0);

    skSetContext1i(SK_PROJECTION_TYPE, SK_CARTESIAN);
    AssertEqualI(SK_PROJECTION_TYPE, SK_CARTESIAN);
    AssertEqualI(SK_Y_UP, 1);

    // make sure it stays the same when
    // it's set to an invalid value
    skSetContext1i(SK_PROJECTION_TYPE, -1);
    AssertEqualI(SK_PROJECTION_TYPE, SK_CARTESIAN);

    skSetContext1i(SK_PROJECTION_TYPE, 2);
    AssertEqualI(SK_PROJECTION_TYPE, SK_CARTESIAN);
    skDeleteContext(ctx);
}

TEST_CASE("SK_VERTICES_PER_SEGMENT")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    AssertEqualI(SK_VERTICES_PER_SEGMENT, 16);

    skSetContext1i(SK_VERTICES_PER_SEGMENT, 32);
    AssertEqualI(SK_VERTICES_PER_SEGMENT, 32);

    skSetContext1i(SK_VERTICES_PER_SEGMENT, -1000);
    AssertEqualI(SK_VERTICES_PER_SEGMENT, 3);

    skSetContext1i(SK_VERTICES_PER_SEGMENT, 1000);
    AssertEqualI(SK_VERTICES_PER_SEGMENT, 128);

    skDeleteContext(ctx);
}

TEST_CASE("SK_OPACITY")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    AssertEqualI(SK_OPACITY, 255);
    AssertEqualF(SK_OPACITY, 1.f);

    skSetContext1i(SK_OPACITY, 128);

    AssertEqualI(SK_OPACITY, 128);
    AssertEqualF(SK_OPACITY, 128.f / 255.f);

    skSetContext1i(SK_OPACITY, -55555555);
    AssertEqualI(SK_OPACITY, 0);

    skSetContext1i(SK_OPACITY, 55555555);
    AssertEqualI(SK_OPACITY, 255);

    skDeleteContext(ctx);
}

TEST_CASE("GetWorkingPaint")
{
    // Test working paint creation / selection / deletion
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    SKpaint p1, p2;
    p1 = skGetWorkingPaint();
    EXPECT_NE(p1, nullptr);

    skSelectPaint(nullptr);
    p1 = skGetWorkingPaint();
    EXPECT_NE(p1, nullptr);

    p2 = skNewPaint();
    skSelectPaint(p2);
    EXPECT_EQ(skGetWorkingPaint(), p2);

    skSelectPaint(nullptr);
    EXPECT_EQ(skGetWorkingPaint(), p1);

    skSelectPaint(p2);
    skDeletePaint(p2);
    // tests 'dangling' reference in ~skContext()
    skDeleteContext(ctx);
}

TEST_CASE("PaintDefaults")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    // default
    AssertPaintEqualI(SK_BRUSH_MODE, SK_BM_REPLACE);
    skSetPaint1i(SK_BRUSH_MODE, SK_BM_ADD);
    AssertPaintEqualI(SK_BRUSH_MODE, SK_BM_ADD);
    skSetPaint1i(SK_BRUSH_MODE, SK_BM_MODULATE);
    AssertPaintEqualI(SK_BRUSH_MODE, SK_BM_MODULATE);
    skSetPaint1i(SK_BRUSH_MODE, SK_BM_SUBTRACT);
    AssertPaintEqualI(SK_BRUSH_MODE, SK_BM_SUBTRACT);
    skSetPaint1i(SK_BRUSH_MODE, SK_BM_DIVIDE);
    AssertPaintEqualI(SK_BRUSH_MODE, SK_BM_DIVIDE);
    skSetPaint1i(SK_BRUSH_MODE, -10000);
    AssertPaintEqualI(SK_BRUSH_MODE, SK_BM_REPLACE);
    skSetPaint1i(SK_BRUSH_MODE, 10000);
    AssertPaintEqualI(SK_BRUSH_MODE, SK_BM_DIVIDE);

    AssertPaintEqualI(SK_BRUSH_STYLE, SK_BS_SOLID);
    skSetPaint1i(SK_BRUSH_STYLE, SK_BS_PATTERN);
    AssertPaintEqualI(SK_BRUSH_STYLE, SK_BS_PATTERN);
    skSetPaint1i(SK_BRUSH_STYLE, -10000);
    AssertPaintEqualI(SK_BRUSH_STYLE, SK_BS_SOLID);
    skSetPaint1i(SK_BRUSH_STYLE, 10000);
    AssertPaintEqualI(SK_BRUSH_STYLE, SK_BS_PATTERN);

    AssertPaintEqualI(SK_LINE_TYPE, SK_LINE_LOOP);
    skSetPaint1i(SK_LINE_TYPE, SK_LINE_LIST);
    AssertPaintEqualI(SK_LINE_TYPE, SK_LINE_LIST);
    skSetPaint1i(SK_LINE_TYPE, SK_LINE_LOOP);
    AssertPaintEqualI(SK_LINE_TYPE, SK_LINE_LOOP);
    skSetPaint1i(SK_LINE_TYPE, SK_POINTS);
    AssertPaintEqualI(SK_LINE_TYPE, SK_POINTS);
    skSetPaint1i(SK_LINE_TYPE, -10000);
    AssertPaintEqualI(SK_LINE_TYPE, SK_LINE_LOOP);
    skSetPaint1i(SK_LINE_TYPE, 10000);
    AssertPaintEqualI(SK_LINE_TYPE, SK_POINTS);

    AssertPaintEqualI(SK_PEN_STYLE, SK_PS_SOLID);
    skSetPaint1i(SK_PEN_STYLE, SK_PS_DASHED);
    AssertPaintEqualI(SK_PEN_STYLE, SK_PS_DASHED);
    skSetPaint1i(SK_PEN_STYLE, SK_PS_SOLID);
    AssertPaintEqualI(SK_PEN_STYLE, SK_PS_SOLID);
    skSetPaint1i(SK_PEN_STYLE, -10000);
    AssertPaintEqualI(SK_PEN_STYLE, SK_PS_SOLID);
    skSetPaint1i(SK_PEN_STYLE, 10000);
    AssertPaintEqualI(SK_PEN_STYLE, SK_PS_DASHED);

    AssertPaintEqualI(SK_PEN_WIDTH, 1);
    skSetPaint1f(SK_PEN_WIDTH, 1.5f);
    AssertPaintEqualF(SK_PEN_WIDTH, 1.5f);
    skSetPaint1f(SK_PEN_WIDTH, -100);
    AssertPaintEqualF(SK_PEN_WIDTH, 0);
    skSetPaint1f(SK_PEN_WIDTH, 100);
    AssertPaintEqualF(SK_PEN_WIDTH, 100);

    AssertPaintEqualI(SK_AUTO_CLEAR, 0);
    skSetPaint1i(SK_AUTO_CLEAR, 1);
    AssertPaintEqualI(SK_AUTO_CLEAR, 1);
    skSetPaint1i(SK_AUTO_CLEAR, -1000);
    AssertPaintEqualI(SK_AUTO_CLEAR, 1);
    skSetPaint1i(SK_AUTO_CLEAR, 1000);
    AssertPaintEqualI(SK_AUTO_CLEAR, 1);

    AssertPaintEqualUI(SK_BRUSH_COLOR, CS_Grey10);
    skSetPaint1ui(SK_BRUSH_COLOR, CS_Grey05);
    AssertPaintEqualUI(SK_BRUSH_COLOR, CS_Grey05);

    AssertPaintEqualUI(SK_SURFACE_COLOR, CS_Grey10);
    skColor3f(0.502f, 0.502f, 0.502f);
    AssertPaintEqualUI(SK_SURFACE_COLOR, 0x808080FF);

    skColor4f(0.502f, 0.502f, 0.502f, 0.502f);
    AssertPaintEqualUI(SK_SURFACE_COLOR, 0x80808080);

    skColor3ub(72, 72, 72);
    AssertPaintEqualUI(SK_SURFACE_COLOR, 0x484848FF);

    skColor3ub(173, 72, 0);
    AssertPaintEqualUI(SK_SURFACE_COLOR, 0xAD4800FF);

    skColor4ub(72, 173, 128, 23);
    AssertPaintEqualUI(SK_SURFACE_COLOR, 0x48AD8017);

    skDeleteContext(ctx);
}

TEST_CASE("PaintSelectDefaults")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);
    // test selecting and using objects

    SKpaint p1 = skNewPaint();
    SKpaint p2 = skNewPaint();

    skSelectPaint(p1);
    skColor1ui(CS_BaseGreen);

    skSelectPaint(p2);
    skColor1ui(CS_BaseRed);

    skSelectPaint(nullptr);

    // default
    AssertPaintEqualUI(SK_SURFACE_COLOR, CS_Grey10);
    skSelectPaint(p1);
    AssertPaintEqualUI(SK_SURFACE_COLOR, CS_BaseGreen);
    skSelectPaint(p2);
    AssertPaintEqualUI(SK_SURFACE_COLOR, CS_BaseRed);
    skSelectPaint(nullptr);
    AssertPaintEqualUI(SK_SURFACE_COLOR, CS_Grey10);

    skDeletePaint(p1);
    skDeletePaint(p2);
    skDeleteContext(ctx);
}

TEST_CASE("GetWorkingPath")
{
    // Test working path creation / selection / deletion
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    SKpath p1, p2;

    p1 = skGetWorkingPath();
    EXPECT_NE(p1, nullptr);

    skSelectPath(nullptr);
    p1 = skGetWorkingPath();
    EXPECT_NE(p1, nullptr);

    p2 = skNewPath();
    skSelectPath(p2);
    EXPECT_EQ(skGetWorkingPath(), p2);

    skSelectPath(nullptr);
    EXPECT_EQ(skGetWorkingPath(), p1);

    skSelectPath(p2);
    skDeletePath(p2);

    // tests 'dangling' reference in ~skContext()
    skDeleteContext(ctx);
}

/*
SK_API void skSetImage1i(SKimage image, SKimageOptionEnum en, SKint32 v);
SK_API void skGetImage1i(SKimage image, SKimageOptionEnum en, SKint32* v);
SK_API void skSetImage1f(SKimage image, SKimageOptionEnum en, SKscalar v);
SK_API void skGetImage1f(SKimage image, SKimageOptionEnum en, SKscalar* v);
*/

void AssertImageEqualI(SKimage image, SKint32 option, const SKint32 expected)
{
    SKint32 prop;
    prop = SK_NO_STATUS;
    skGetImage1i(image, option, &prop);
    EXPECT_EQ(expected, prop);
}

TEST_CASE("ImageTest")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    SKimage image;
    image = skNewImage();
    AssertImageEqualI(image, SK_IMAGE_FILTER, SK_FILTER_NONE);
    AssertImageEqualI(image, SK_IMAGE_MIPMAP, 0);
    AssertImageEqualI(image, SK_IMAGE_WIDTH, SK_NO_STATUS);
    AssertImageEqualI(image, SK_IMAGE_HEIGHT, SK_NO_STATUS);
    AssertImageEqualI(image, SK_IMAGE_PITCH, SK_NO_STATUS);
    AssertImageEqualI(image, SK_IMAGE_BPP, SK_NO_STATUS);
    AssertImageEqualI(image, SK_IMAGE_SIZE_IN_BYTES, SK_NO_STATUS);
    AssertImageEqualI(image, SK_IMAGE_PIXEL_FORMAT, SK_NO_STATUS);
    skDeleteImage(image);

    image = skCreateImage(64, 64, SK_RGB);
    AssertImageEqualI(image, SK_IMAGE_FILTER, SK_FILTER_NONE);
    AssertImageEqualI(image, SK_IMAGE_MIPMAP, 0);
    AssertImageEqualI(image, SK_IMAGE_WIDTH, 64);
    AssertImageEqualI(image, SK_IMAGE_HEIGHT, 64);
    AssertImageEqualI(image, SK_IMAGE_PITCH, 64 * 3);
    AssertImageEqualI(image, SK_IMAGE_BPP, 3);
    AssertImageEqualI(image, SK_IMAGE_SIZE_IN_BYTES, 64 * 64 * 3);
    AssertImageEqualI(image, SK_IMAGE_PIXEL_FORMAT, SK_RGB);
    skDeleteImage(image);

    image = skImageLoad("test1.png");
    AssertImageEqualI(image, SK_IMAGE_FILTER, SK_FILTER_NONE);
    AssertImageEqualI(image, SK_IMAGE_MIPMAP, 0);
    AssertImageEqualI(image, SK_IMAGE_WIDTH, 300);
    AssertImageEqualI(image, SK_IMAGE_HEIGHT, 295);
    AssertImageEqualI(image, SK_IMAGE_PITCH, 300 * 4);
    AssertImageEqualI(image, SK_IMAGE_BPP, 4);
    AssertImageEqualI(image, SK_IMAGE_SIZE_IN_BYTES, 300 * 295 * 4);
    AssertImageEqualI(image, SK_IMAGE_PIXEL_FORMAT, SK_RGBA);
    skDeleteImage(image);

    skDeleteContext(ctx);
}

void AssertFontEqualI(SKfont font, SKint32 option, const SKint32 expected)
{
    SKint32 prop;
    prop = SK_NO_STATUS;
    skGetFont1i(font, option, &prop);
    EXPECT_EQ(expected, prop);
}

void AssertFontEqualF(SKfont font, SKint32 option, const SKscalar expected)
{
    SKscalar prop;
    prop = SK_NO_STATUS;
    skGetFont1f(font, option, &prop);
    EXPECT_TRUE(feq(expected, prop));
}

TEST_CASE("FontLoadTest")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    SKfont font;
    font = skNewFont(SK_FONT_DEFAULT_FIXED, 24, 72);
    EXPECT_NE(font, nullptr);
    AssertFontEqualI(font, SK_FONT_DPI, 72);
    AssertFontEqualI(font, SK_FONT_PIXEL_SIZE, 24);
    skDeleteFont(font);

    font = skNewFont(SK_FONT_DEFAULT, 24, 72);
    EXPECT_NE(font, nullptr);
    AssertFontEqualI(font, SK_FONT_DPI, 72);
    AssertFontEqualI(font, SK_FONT_PIXEL_SIZE, 24);
    skDeleteFont(font);

    font = skNewFont(SK_FONT_SPC1, 24, 72);
    EXPECT_NE(font, nullptr);
    AssertFontEqualI(font, SK_FONT_DPI, 72);
    AssertFontEqualI(font, SK_FONT_PIXEL_SIZE, 24);
    skDeleteFont(font);

    font = skNewFont(SK_FONT_SPC2, 24, 72);
    EXPECT_NE(font, nullptr);
    AssertFontEqualI(font, SK_FONT_DPI, 72);
    AssertFontEqualI(font, SK_FONT_PIXEL_SIZE, 24);
    skDeleteFont(font);

    font = skNewFont(SK_FONT_UI, 24, 72);
    EXPECT_NE(font, nullptr);
    AssertFontEqualI(font, SK_FONT_DPI, 72);
    AssertFontEqualI(font, SK_FONT_PIXEL_SIZE, 24);
    skDeleteFont(font);

    font = skNewFont(SK_FONT_UI_LIGHT, 24, 72);
    EXPECT_NE(font, nullptr);
    AssertFontEqualI(font, SK_FONT_DPI, 72);
    AssertFontEqualI(font, SK_FONT_PIXEL_SIZE, 24);
    skDeleteFont(font);

    skDeleteContext(ctx);
}

TEST_CASE("FontRangeTest")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    // Input is clamped to DPI(SK_MIN_DPI, SK_MAX_DPI)
    //                     Size(SK_MIN_SIZE, SK_MAX_SIZE)

    SKfont font;
    font = skNewFont(SK_FONT_DEFAULT, 0, 0);
    EXPECT_NE(font, nullptr);
    AssertFontEqualI(font, SK_FONT_DPI, 24);
    AssertFontEqualI(font, SK_FONT_PIXEL_SIZE, 8);
    skDeleteFont(font);

    font = skNewFont(SK_FONT_DEFAULT, 88888, 7545681);
    EXPECT_NE(font, nullptr);
    AssertFontEqualI(font, SK_FONT_DPI, 300);
    AssertFontEqualI(font, SK_FONT_PIXEL_SIZE, 96);
    skDeleteFont(font);

    skDeleteContext(ctx);
}

TEST_CASE("FontPropertyTest")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);

    SKfont font;
    font = skNewFont(SK_FONT_DEFAULT, 0, 0);
    AssertFontEqualI(font, SK_FONT_DPI, 24);
    AssertFontEqualI(font, SK_FONT_PIXEL_SIZE, 8);
    EXPECT_NE(font, nullptr);
   
    // read only after creation
    skSetFont1i(font, SK_FONT_DPI, 300);
    AssertFontEqualI(font, SK_FONT_DPI, 24);
    skSetFont1i(font, SK_FONT_PIXEL_SIZE, 300);
    AssertFontEqualI(font, SK_FONT_PIXEL_SIZE, 8);

    AssertFontEqualF(font, SK_FONT_SIZE, 8);
    AssertFontEqualI(font, SK_FONT_SIZE, 8);

    skSetFont1f(font, SK_FONT_SIZE, 300);
    AssertFontEqualI(font, SK_FONT_SIZE, 300);
    AssertFontEqualI(font, SK_FONT_RELATIVE_SCALE, 300 / ((24 * 8) >> 6));

    AssertFontEqualI(font, SK_FONT_FILTER, SK_FILTER_BI_LINEAR);
    AssertFontEqualI(font, SK_FONT_MIPMAP, 0);

    skSetFont1i(font, SK_FONT_FILTER, -1);
    AssertFontEqualI(font, SK_FONT_FILTER, SK_FILTER_NONE);
    skSetFont1i(font, SK_FONT_FILTER, 9999);
    AssertFontEqualI(font, SK_FONT_FILTER, SK_FILTER_BI_LINEAR);
    skSetFont1i(font, SK_FONT_FILTER, SK_FILTER_NONE);
    AssertFontEqualI(font, SK_FONT_FILTER, SK_FILTER_NONE);

    skDeleteFont(font);
    skDeleteContext(ctx);
}
