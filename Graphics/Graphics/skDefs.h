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
#ifndef _skDefinitions_h_
#define _skDefinitions_h_

#include "Math/skColor.h"
#include "Math/skMatrix4.h"
#include "Utils/skAllocator.h"
#include "skGraphics.h"

class skContext;
class skRenderer;
class skPaint;
class skPath;
class skTexture;
class skFont;

struct SKcontextOptions
{
    SKint32     verticesPerIteration;
    SKint32     polygonStroke;
    skColor     clearColor;
    skRectangle clearRectangle;
    skVector2   contextSize;
    skVector2   contextScale;
    skVector2   contextBias;
    skScalar    opacity;
    SKmetrics   metrics;
    skRectangle metricsRect;
    SKint32     flipViewport;
    SKint32     currentViewport;
    SKint32     defaultFont;
    bool        yIsUp;
};

#define SK_TEXTURE(x) reinterpret_cast<skTexture*>((x))
#define SK_FONT(x) reinterpret_cast<skFont*>((x))
#define SK_CONTEXT(x) reinterpret_cast<skContext*>((x))
#define SK_TO_HANDLE(x, h) reinterpret_cast<h>((x))
#define SK_IMAGE_HANDLE(x) SK_TO_HANDLE(x, SKimage)
#define SK_FONT_HANDLE(x) SK_TO_HANDLE(x, SKfont)
#define SK_CONTEXT_HANDLE(x) SK_TO_HANDLE(x, SKcontext)
#define SK_CSTRING_HANDLE(x) SK_TO_HANDLE(x, SKcacheString)



template <typename Ret, typename H, typename C>
Ret* SKcheckType(H inp, C* ctx)
{
    Ret* R = reinterpret_cast<Ret*>(inp);
    if (!R || R->getContext() != ctx)
        return nullptr;
    return R;
}

#endif  //_skDefinitions_h_
