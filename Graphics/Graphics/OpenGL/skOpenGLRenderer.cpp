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
#include "OpenGL/skOpenGLRenderer.h"
#include "OpenGL/skOpenGLTexture.h"
#include "OpenGL/skOpenGLVertexBuffer.h"
#include "OpenGL/skProgram.h"
#include "Pipeline/ColoredFragment.inl"
#include "Pipeline/ColoredVertex.inl"
#include "Pipeline/FontFragment.inl"
#include "Pipeline/TexturedFragment.inl"
#include "Pipeline/TexturedVertex.inl"
#include "Utils/skPlatformHeaders.h"
#include "Window/OpenGL/skOpenGL.h"
#include "skCachedProgram.h"
#include "skCachedString.h"
#include "skContext.h"
#include "skContour.h"
#include "skFont.h"
#include "skPaint.h"
#include "skPath.h"

const GLint clear_bits = GL_COLOR_BUFFER_BIT;

skOpenGLRenderer::skOpenGLRenderer() :
    m_projection(skMatrix4::Identity),
    m_defaultShader(new skCachedProgram()),
    m_fontShader(new skCachedProgram()),
    m_blankShader(new skCachedProgram()),
    m_viewport(0, 0, 0, 0),
    m_fontPath(new skPath()),
    m_curPath(nullptr),
    m_curPaint(nullptr),
    m_fillOp(GL_TRIANGLE_STRIP)
{
    compileBuiltin();
}

skOpenGLRenderer::~skOpenGLRenderer()
{
    delete m_fontPath;
    delete m_defaultShader;
    delete m_fontShader;
    delete m_blankShader;
}

void skOpenGLRenderer::compileBuiltin(void) const
{
    m_defaultShader->compile(TexturedVertex,
                             TexturedFragment);
    m_defaultShader->bindAttribute("position", SK_ATTR_POSITION);
    m_defaultShader->bindAttribute("textureCoords", SK_ATTR_TEXTURE0);

    m_fontShader->compile(TexturedVertex,
                          FontFragment);

    m_fontShader->bindAttribute("position", SK_ATTR_POSITION);
    m_fontShader->bindAttribute("textureCoords", SK_ATTR_TEXTURE0);

    m_blankShader->compile(ColoredVertex,
                           ColoredFragment);
    m_blankShader->bindAttribute("position", SK_ATTR_POSITION);
    m_blankShader->bindAttribute("textureCoords", SK_ATTR_TEXTURE0);
}

void skOpenGLRenderer::projectBox(skScalar x1, skScalar y1, skScalar x2, skScalar y2)
{
    skMath::ortho2D(m_projection, x1, y1, x2, y2);

    skMatrix4 tm;
    tm.makeIdentity();
    tm.setTrans(0.375f, 0.375f, 0.f);
    m_projection = m_projection * tm;
}

void skOpenGLRenderer::loadRect(const skRectangle& rect)
{
    skMath::ortho2D(m_projection, rect.getLeft(), rect.getTop(), rect.getRight(), rect.getBottom());

    skMatrix4 tm;
    tm.makeIdentity();
    tm.setTrans(0.375f, 0.375f, 0.f);
    m_projection = m_projection * tm;
}

void skOpenGLRenderer::clear(void)
{
    const skContext& ctx = ref();
    skRectangle      rect;
    rect.setPosition(0, 0);
    rect.setSize(ctx.getSize());
    clear(rect);
}

void skOpenGLRenderer::clear(const skRectangle& rect)
{
    const skContext& ctx   = ref();
    const skColor&   clear = ctx.getContextC(SK_CLEAR_COLOR);

    if (ctx.getContextI(SK_USE_CURRENT_VIEWPORT))
    {
        glClearColor(
            (float)clear.r,
            (float)clear.g,
            (float)clear.b,
            (float)clear.a);
        glClear(clear_bits);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        return;
    }

    const SKint32 x = (SKint32)rect.x;
    const SKint32 y = (SKint32)rect.y;
    const SKint32 w = (SKint32)rect.width;
    const SKint32 h = (SKint32)rect.height;

    glViewport(x, y, w, h);
    glClearColor(
        (float)clear.r,
        (float)clear.g,
        (float)clear.b,
        (float)clear.a);
    glClear(clear_bits);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void skOpenGLRenderer::doPolyFill(void) const
{
    const skContext& ctx = ref();

    if (!m_curPaint->m_program)
        return;

    const bool lines = m_fillOp == GL_LINES || m_fillOp == GL_LINE_STRIP;

    m_curPaint->m_program->enable(true);
    m_curPaint->m_program->setMode(m_curPaint->m_brushMode);
    m_curPaint->m_program->setZOrder(0);

    if (m_curPaint->m_brushPattern && !lines)
    {
        m_curPath->makeUV();

        skOpenGLTexture* ima = (skOpenGLTexture*)m_curPaint->m_brushPattern;

        glEnableTexture2D();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ima->getImage());

        m_curPaint->m_program->setImage(0);
    }

    if (lines && m_curPaint->m_penWidth > 1)
    {
        glEnableLineSmooth();
        glLineWidth(m_curPaint->m_penWidth);
    }

    const skScalar& opacity = ref().getContextF(SK_OPACITY);

    skScalar col[] = {
        m_curPaint->m_surfaceColor.r,
        m_curPaint->m_surfaceColor.g,
        m_curPaint->m_surfaceColor.b,
        m_curPaint->m_surfaceColor.a * opacity,
    };

    m_curPaint->m_program->setSurface(col);

    if (m_curPaint->m_brushMode != SK_BM_REPLACE)
    {
        col[0] = m_curPaint->m_brushColor.r;
        col[1] = m_curPaint->m_brushColor.g;
        col[2] = m_curPaint->m_brushColor.b;
        col[3] = m_curPaint->m_brushColor.a * opacity;
        m_curPaint->m_program->setBrush(col);
    }

    m_curPaint->m_program->setViewProj((m_projection * ctx.getMatrix()).p);

    if (m_curPath->getBuffer())
        m_curPath->getBuffer()->fill(m_fillOp);

    m_curPaint->m_program->enable(false);

    if (m_curPaint->m_brushPattern && !lines)
        glDisableTexture2D();

    if (lines && m_curPaint->m_penWidth > 1)
    {
        glDisableLineSmooth();
        glLineWidth(1);
    }
}

bool skOpenGLRenderer::shouldBlend() const
{
    bool blend = m_ctx->getContextF(SK_OPACITY) < 1.f;
    if (!blend)
        blend = m_curPaint->m_surfaceColor.a < 1.f || m_curPaint->m_brushPattern;
    if (!blend)
        blend = m_curPaint->m_brushMode != SK_BM_REPLACE;
    if (!blend)
        blend = m_curPaint->m_penWidth > 1.f;
    return blend;
}

void skOpenGLRenderer::fill(skPath* pth)
{
    m_curPath = pth;

    if (m_fillOp != GL_TRIANGLE_STRIP && m_fillOp != GL_TRIANGLES)
        m_fillOp = GL_TRIANGLE_FAN;
    else if (m_curPaint->m_lineType == SK_POINTS)
    {
#if SK_PLATFORM != SK_PLATFORM_EMSCRIPTEN
        glPointSize(m_curPaint->m_penWidth);
        m_fillOp = GL_POINTS;
#else
        // Not supported using WebGL
        m_fillOp = GL_LINES;
#endif
    }

    const bool blend = shouldBlend();
    if (blend)
        glEnable(GL_BLEND);

    doPolyFill();

    if (blend)
        glDisable(GL_BLEND);

    m_fillOp = 0;
}

void skOpenGLRenderer::stroke(skPath* pth)
{
    m_curPath = pth;
    m_fillOp  = GL_LINE_STRIP;

    if (m_curPaint->m_lineType == SK_LINE_LIST)
        m_fillOp = GL_LINES;
    else if (m_curPaint->m_lineType == SK_POINTS)
    {
#if SK_PLATFORM != SK_PLATFORM_EMSCRIPTEN
        glPointSize(m_curPaint->m_penWidth);
        m_fillOp = GL_POINTS;
#else
        // Not supported using WebGL
        m_fillOp = GL_LINES;
#endif
    }

    const bool blend = shouldBlend();
    if (blend)
        glEnable(GL_BLEND);

    doPolyFill();

    if (blend)
        glDisable(GL_BLEND);
    m_fillOp = 0;
}

void skOpenGLRenderer::displayString(skFont*     font,
                                     const char* str,
                                     SKuint32    len,
                                     skScalar    x,
                                     skScalar    y)
{
    SK_CHECK_PARAM(font, SK_RETURN_VOID);
    SK_CHECK_PARAM(len, SK_RETURN_VOID);
    SK_CHECK_PARAM(m_curPaint, SK_RETURN_VOID);

    font->buildPath(m_fontPath, str, len, x, y);

    m_curPaint->m_brushPattern = font->getImage();
    m_curPaint->m_program      = m_fontShader;

    m_fillOp = GL_TRIANGLES;
    fill(m_fontPath);

    m_curPaint->m_brushPattern = nullptr;
    m_curPaint->m_program      = nullptr;
}

void skOpenGLRenderer::displayString(skCachedString* str)
{
    SK_CHECK_PARAM(str, SK_RETURN_VOID);
    SK_CHECK_PARAM(m_ctx, SK_RETURN_VOID);

    skFont* fnt = m_ctx->getWorkFont();

    SK_CHECK_PARAM(fnt, SK_RETURN_VOID);
    skOpenGLTexture* img = (skOpenGLTexture*)fnt->getImage();
    SK_CHECK_PARAM(img, SK_RETURN_VOID);

    m_curPaint->m_brushPattern = img;
    m_curPaint->m_program      = m_fontShader;

    m_fillOp = GL_TRIANGLES;
    fill(str->getPath());

    m_curPaint->m_brushPattern = nullptr;
    m_curPaint->m_program      = nullptr;
}

void skOpenGLRenderer::selectPaint(skPaint* paint)
{
    m_curPaint = paint;
    if (m_curPaint)
    {
        m_curPaint->m_program = m_defaultShader;

        if (!m_curPaint->m_brushPattern)
            m_curPaint->m_program = m_blankShader;
    }
}
