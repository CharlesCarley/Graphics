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

    SKint32 prop;

    prop = SK_NO_STATUS;
    skGetContext1i(SK_USE_CURRENT_VIEWPORT, &prop);
    EXPECT_EQ(prop, 0);

    prop = SK_NO_STATUS;
    skGetContext1i(SK_METRICS_MODE, &prop);
    EXPECT_EQ(prop, SK_PIXEL);

    prop = SK_NO_STATUS;
    skGetContext1i(SK_Y_UP, &prop);
    EXPECT_EQ(prop, 0);

    skDeleteContext(ctx);
}

TEST_CASE("SK_PROJECTION_TYPE")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);
    SKint32   prop;
    prop = SK_NO_STATUS;
    skGetContext1i(SK_PROJECTION_TYPE, &prop);
    EXPECT_EQ(prop, SK_STANDARD);

    prop = SK_NO_STATUS;
    skGetContext1i(SK_Y_UP, &prop);
    EXPECT_EQ(prop, 0);

    skSetContext1i(SK_PROJECTION_TYPE, SK_CARTESIAN);

    prop = SK_NO_STATUS;
    skGetContext1i(SK_PROJECTION_TYPE, &prop);
    EXPECT_EQ(prop, SK_CARTESIAN);

    prop = SK_NO_STATUS;
    skGetContext1i(SK_Y_UP, &prop);
    EXPECT_EQ(prop, 1);

    // make sure it stays the same when it's set to an invalid value
    skSetContext1i(SK_PROJECTION_TYPE, -1);
    prop = SK_NO_STATUS;
    skGetContext1i(SK_PROJECTION_TYPE, &prop);
    EXPECT_EQ(prop, SK_CARTESIAN);

    skSetContext1i(SK_PROJECTION_TYPE, 2);
    prop = SK_NO_STATUS;
    skGetContext1i(SK_PROJECTION_TYPE, &prop);
    EXPECT_EQ(prop, SK_CARTESIAN);

    skDeleteContext(ctx);
}

TEST_CASE("SK_VERTICES_PER_SEGMENT")
{
    SKcontext ctx = skNewBackEndContext(SK_BE_None);
    SKint32   prop;
    prop = SK_NO_STATUS;
    skGetContext1i(SK_VERTICES_PER_SEGMENT, &prop);
    EXPECT_EQ(prop, 16);

    skSetContext1i(SK_VERTICES_PER_SEGMENT, 32);
    prop = SK_NO_STATUS;
    skGetContext1i(SK_VERTICES_PER_SEGMENT, &prop);
    EXPECT_EQ(prop, 32);

    skSetContext1i(SK_VERTICES_PER_SEGMENT, -1000);
    prop = SK_NO_STATUS;
    skGetContext1i(SK_VERTICES_PER_SEGMENT, &prop);
    EXPECT_EQ(prop, 3);

    skSetContext1i(SK_VERTICES_PER_SEGMENT, 1000);
    prop = SK_NO_STATUS;
    skGetContext1i(SK_VERTICES_PER_SEGMENT, &prop);
    EXPECT_EQ(prop, 128);

    skDeleteContext(ctx);
}
