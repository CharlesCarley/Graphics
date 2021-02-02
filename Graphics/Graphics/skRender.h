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
#ifndef _skRender_h_
#define _skRender_h_

#include "skContextObject.h"

class skPath;
class skPaint;
class skCachedString;

class skRenderer : public skContextObj
{
public:
    skRenderer() = default;

    virtual ~skRenderer() = default;

    virtual void clear(void) = 0;

    virtual void clear(const skRectangle& rect) = 0;

    virtual void fill(skPath* pth) = 0;

    virtual void stroke(skPath* pth) = 0;

    virtual void selectPaint(skPaint* paint) = 0;

    virtual void flush(void) = 0;

    virtual void projectBox(skScalar x1, skScalar y1, skScalar x2, skScalar y2) = 0;

    virtual void displayString(skFont* font, const char* str, SKuint32 len, skScalar x, skScalar y) = 0;

    virtual void displayString(skCachedString* str) = 0;
};



#endif  //_skRender_h_
