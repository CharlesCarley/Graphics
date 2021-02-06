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
    p1= skGetWorkingPaint();
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

TEST_CASE("GetWorkingPath")
{
    // Test working paint creation / selection / deletion
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
