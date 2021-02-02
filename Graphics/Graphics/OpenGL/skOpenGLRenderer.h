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
#ifndef _skOpenGLRenderer_h_
#define _skOpenGLRenderer_h_


#include "skRender.h"

class skVertexBuffer;
class skCachedProgram;
class skCachedString;

class skOpenGLRenderer : public skRenderer
{
private:
    skMatrix4        m_projection;
    skCachedProgram* m_defShader;
    skCachedProgram* m_fntShader;
    skCachedProgram* m_bnkShader;
    skRectangle      m_viewport;
    skPath*          m_fontPath;
    skPath*          m_curPath;
    skPaint*         m_curPaint;
    SKint32          m_fillOp;
    SKint32          m_checkScissor;

public:
    skOpenGLRenderer();
    ~skOpenGLRenderer() override;

    void clear(void) override;

    void clear(const skRectangle& rect) override;

    void flush(void) override;

    void fill(skPath* pth) override;

    void stroke(skPath* pth) override;

    void selectPaint(skPaint* paint) override;

    void projectBox(skScalar x1, skScalar y1, skScalar x2, skScalar y2) override;

    void displayString(skFont* font, const char* str, SKuint32 len, skScalar x, skScalar y) override;

    void displayString(skCachedString* str) override;

private:
    void doPolyFill(void) const;

    void loadRect(const skRectangle& rect);

    void compileBuiltin(void) const;
};


#endif  //_skOpenGLRenderer_h_
