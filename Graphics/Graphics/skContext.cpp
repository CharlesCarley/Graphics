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
#include "skContext.h"
#include <cstdio>

#ifdef Graphics_BACKEND_OPENGL
#include "OpenGL/skOpenGLRenderer.h"
#include "OpenGL/skOpenGLTexture.h"
#include "OpenGL/skOpenGLVertexBuffer.h"
#include "Window/OpenGL/skOpenGL.h"
#endif

#include "Utils/skDisableWarnings.h"
#include "Utils/skLogger.h"
#include "skCachedString.h"
#include "skFont.h"
#include "skPaint.h"
#include "skPath.h"
#include "skRender.h"
#include "skTexture.h"

skContext::skContext(SKint32 backend)
{
    static SKint32 _ctxHandle = 0;
    skImage::initialize();

    m_renderContext = nullptr;
    m_id            = _ctxHandle++;
    m_backend       = backend;

    // not owned by the user
    m_workPaint = new skPaint();
    m_workPaint->setContext(this);
    m_workPath = new skPath();
    m_workPath->setContext(this);

    m_tempPaint = nullptr;
    m_tempPath  = nullptr;
    m_workFont  = nullptr;

    m_matrix.makeIdentity();
    m_options.verticesPerSegment = SK_DEFAULT_VERTICES_PER_SEGMENT;
    m_options.clearColor         = skColor(0, 0, 0, 1);
    m_options.clearRectangle     = skRectangle(0, 0, 1, 1);
    m_options.contextSize        = skVector2::Unit;
    m_options.contextScale       = skVector2::Unit;
    m_options.contextBias        = skVector2::Zero;
    m_options.opacity            = 1.f;
    m_options.metrics            = SK_DEFAULT_METRICS_MODE;
    m_options.currentViewport    = 0;
    m_options.projectionType     = SK_DEFAULT_PROJECTION_MODE;
    m_options.yIsUp              = false;

#ifdef Graphics_BACKEND_OPENGL
    if (m_backend == SK_BE_OpenGL)
        makeCurrent(new skOpenGLRenderer());
#endif
}

skContext::~skContext()
{
    if (m_tempPaint)
        selectPaint(nullptr);
    delete m_workPaint;

    if (m_tempPath)
        selectPath(nullptr);
    delete m_workPath;

    delete m_renderContext;

    skImage::finalize();
}

void skContext::makeCurrent(skRenderer* ctx)
{
    if (ctx)
    {
        delete m_renderContext;
        m_renderContext = ctx;
        ctx->setContext(this);
    }
}

SKimage skContext::createImage(SKuint32 w, SKuint32 h, SKpixelFormat fmt)
{
    if (m_backend == SK_BE_OpenGL)
    {
#ifdef Graphics_BACKEND_OPENGL
        skOpenGLTexture* tex = new skOpenGLTexture(w, h, fmt);

        tex->setContext(this);
        return SK_IMAGE_HANDLE(tex);
#endif
    }

    if (m_backend == SK_BE_None)
    {
        skTexture* tex = new skTexture(w, h, fmt);
        tex->setContext(this);
        return SK_IMAGE_HANDLE(tex);
    }
    return nullptr;
}

SKimage skContext::newImage()
{
    if (m_backend == SK_BE_OpenGL)
    {
#ifdef Graphics_BACKEND_OPENGL
        skOpenGLTexture* ima = new skOpenGLTexture();
        ima->setContext(this);
        return SK_IMAGE_HANDLE(ima);
#endif
    }

    if (m_backend == SK_BE_None)
    {
        skTexture* tex = new skTexture();

        tex->setContext(this);
        return SK_IMAGE_HANDLE(tex);
    }

    return nullptr;
}

void skContext::displayString(skCachedString* str) const
{
    if (m_renderContext && str)
    {
        m_renderContext->selectPaint(m_workPaint);
        m_renderContext->displayString(str);
        m_renderContext->selectPaint(nullptr);
    }
}

skTexture* skContext::createInternalImage(SKuint32 w, SKuint32 h, SKpixelFormat fmt)
{
    if (m_backend == SK_BE_OpenGL)
    {
#ifdef Graphics_BACKEND_OPENGL
        skOpenGLTexture* tex = new skOpenGLTexture(w, h, fmt);

        tex->setContext(this);
        return tex;
#endif
    }

    if (m_backend == SK_BE_None)
    {
        skTexture* tex = new skTexture(w, h, fmt);

        tex->setContext(this);
        return tex;
    }

    SK_ASSERT(0);
    return nullptr;
}

void skContext::deleteImage(SKimage ima)
{
    skTexture* img = SK_TEXTURE(ima);

    if (!img || img->getContext() != this)
        return;

    delete img;
}

void skContext::selectImage(SKimage ima)
{
    if (m_workPaint)
    {
        skTexture* img = SK_TEXTURE(ima);
        if (img && img->getContext() != this)
            return;

        m_workPaint->setT(SK_BRUSH_PATTERN, img);
    }
}

SKfont skContext::newFont(SKbuiltinFont font, SKuint32 size, SKuint32 dpi)
{
    dpi  = skClamp<SKuint32>(dpi, SK_MIN_DPI, SK_MAX_DPI);
    size = skClamp<SKuint32>(size, SK_MIN_FONT_SIZE, SK_MAX_FONT_SIZE);

    skFont* fnt = new skFont();
    fnt->setContext(this);

    if (!fnt->fromEnum(font, size, dpi))
    {
        delete fnt;
        return nullptr;
    }

    return SK_FONT_HANDLE(fnt);
}

void skContext::selectFont(SKfont font)
{
    m_workFont = SK_FONT(font);
    if (m_workFont)
        m_workFont->setContext(this);
}

SKfont skContext::newFontFromFile(const char* path, SKuint32 size, SKuint32 dpi)
{
    SK_CHECK_PARAM(path, nullptr);

    dpi  = skClamp<SKuint32>(dpi, SK_MIN_DPI, SK_MAX_DPI);
    size = skClamp<SKuint32>(size, SK_MIN_FONT_SIZE, SK_MAX_FONT_SIZE);

    skFont* fnt = new skFont();
    fnt->setContext(this);

    if (!fnt->fromFile(path, size, dpi))
    {
        delete fnt;
        return nullptr;
    }
    return SK_FONT_HANDLE(fnt);
}

void skContext::deleteFont(SKfont font)
{
    skFont* fnt = SK_FONT(font);
    if (!fnt || fnt->getContext() != this)
        return;
    delete fnt;
}

void skContext::displayString(SKfont font, const char* str, SKuint32 len, skScalar x, skScalar y)
{
    if (m_renderContext)
    {
        skFont* fnt = SK_FONT(font);
        if (!fnt || fnt->getContext() != this)
            return;

        m_renderContext->selectPaint(m_workPaint);
        m_renderContext->displayString(fnt, str, len, x, y);
        m_renderContext->selectPaint(nullptr);
    }
}

void skContext::projectContext(SKprojectionType pt)
{
    if (pt == SK_CARTESIAN)
    {
        m_options.yIsUp   = true;
        const skScalar hx = m_options.contextSize.x * .5f;
        const skScalar hy = m_options.contextSize.y * .5f;
        projectBox(-hx, -hy, hx, hy);
    }
    else
    {
        m_options.yIsUp = false;
        projectRect(0, 0, m_options.contextSize.x, m_options.contextSize.y);
    }
}

void skContext::projectRect(skScalar x, skScalar y, skScalar w, skScalar h) const
{
    projectBox(x, y + h, x + w, y);
}

void skContext::projectBox(skScalar x1, skScalar y1, skScalar x2, skScalar y2) const
{
    if (m_renderContext)
        m_renderContext->projectBox(x1, y1, x2, y2);
}

void skContext::makeRect(const skRectangle& r) const
{
    makeRect(r.x, r.y, r.width, r.height);
}

void skContext::makeRect(const skScalar& x,
                         const skScalar& y,
                         const skScalar& w,
                         const skScalar& h) const
{
    if (m_workPath)
        m_workPath->makeRect(x, y, w, h);
}

void skContext::clearContext(void) const
{
    if (m_renderContext)
        m_renderContext->clear();
}

void skContext::clear(void) const
{
    if (m_renderContext)
        m_renderContext->clear(m_options.clearRectangle);
}

void skContext::selectPath(skPath* pth)
{
    if (pth)
    {
        if (m_workPath && !m_tempPath)
            m_tempPath = m_workPath;

        m_workPath = pth;
        m_workPath->setContext(this);
    }
    else
    {
        if (m_tempPath)
        {
            m_workPath = m_tempPath;
            m_tempPath = nullptr;
        }
        else
            skLogd(LD_WARN, "Attempt to select a null working path\n");
    }
}

void skContext::selectPaint(skPaint* obj)
{
    if (obj)
    {
        if (m_workPaint && !m_tempPaint)
            m_tempPaint = m_workPaint;

        m_workPaint = obj;
        m_workPaint->setContext(this);
    }
    else
    {
        if (m_tempPaint)
        {
            m_workPaint = m_tempPaint;
            m_tempPaint = nullptr;
        }
        else
            skLogd(LD_WARN, "Attempt to select a null working paint\n");
    }
}

void skContext::fill(void) const
{
    if (m_renderContext)
    {
        m_renderContext->selectPaint(m_workPaint);
        m_renderContext->fill(m_workPath);

        if (m_workPaint->autoClear())
            m_workPath->clear();

        m_renderContext->selectPaint(nullptr);
    }
}

void skContext::stroke(void) const
{
    if (m_renderContext)
    {
        m_renderContext->selectPaint(m_workPaint);
        m_renderContext->stroke(m_workPath);

        if (m_workPaint->autoClear())
            m_workPath->clear();

        m_renderContext->selectPaint(nullptr);
    }
}

SKint32 skContext::getContextI(SKcontextOptionEnum op) const
{
    switch (op)
    {
    case SK_VERTICES_PER_SEGMENT:
        return m_options.verticesPerSegment;
    case SK_OPACITY:
        return skClamp<SKint32>(SKint32(m_options.opacity * 255.f), 0, 255);
    case SK_METRICS_MODE:
        return (SKint32)m_options.metrics;
    case SK_USE_CURRENT_VIEWPORT:
        return m_options.currentViewport ? 1 : 0;
    case SK_Y_UP:
        return m_options.yIsUp ? 1 : 0;
    case SK_PROJECTION_TYPE:
        return m_options.projectionType;
    //case SK_CLEAR_COLOR:
    //case SK_CLEAR_RECT:
    //case SK_CONTEXT_SIZE:
    //case SK_CONTEXT_SCALE:
    //case SK_CONTEXT_BIAS:
    default:
        break;
    }
    return SK_NO_STATUS;
}

void skContext::setContextI(SKcontextOptionEnum op, SKint32 v)
{
    switch (op)
    {
    case SK_VERTICES_PER_SEGMENT:
        m_options.verticesPerSegment = skClamp<SKint32>(v,
                                                        SK_MIN_VERTICES_PER_SEGMENT,
                                                        SK_MAX_VERTICES_PER_SEGMENT);
        break;
    case SK_OPACITY:
        m_options.opacity = (skScalar)skClamp<SKint32>(v, 0, 255) / (skScalar)255.f;
        break;
    case SK_METRICS_MODE:
        m_options.metrics = (SKmetricsMode)v;
        break;
    case SK_USE_CURRENT_VIEWPORT:
        m_options.currentViewport = v ? true : false;
        break;
    case SK_Y_UP:
        m_options.yIsUp = v != 0;
        break;
    case SK_PROJECTION_TYPE:
        switch (v)
        {
        case SK_STANDARD:
            m_options.projectionType = SK_STANDARD;
            m_options.yIsUp          = false;
            break;
        case SK_CARTESIAN:
            m_options.projectionType = SK_CARTESIAN;
            m_options.yIsUp          = true;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

skScalar skContext::getContextF(SKcontextOptionEnum op) const
{
    switch (op)
    {
    case SK_VERTICES_PER_SEGMENT:
        return skScalar(m_options.verticesPerSegment);
    case SK_OPACITY:
        return m_options.opacity;
    case SK_METRICS_MODE:
        return (skScalar)m_options.metrics;
    case SK_USE_CURRENT_VIEWPORT:
        return m_options.currentViewport ? skScalar(1.0) : skScalar(0.0);
    case SK_Y_UP:
        return m_options.yIsUp ? skScalar(1.0) : skScalar(0.0);
    default:
        break;
    }
    return SK_NO_STATUS;
}

void skContext::setContextF(SKcontextOptionEnum op, skScalar v)
{
    switch (op)
    {
    case SK_VERTICES_PER_SEGMENT:
        m_options.verticesPerSegment = skClamp<SKint32>(SKint32(v),
                                                        SK_MIN_VERTICES_PER_SEGMENT,
                                                        SK_MAX_VERTICES_PER_SEGMENT);
        break;
    case SK_OPACITY:
        m_options.opacity = skClamp<skScalar>(v, 0.f, 1.f);
        break;
    case SK_METRICS_MODE:
        m_options.metrics = (SKmetricsMode)(SKint32)v;
        break;
    case SK_USE_CURRENT_VIEWPORT:
        m_options.currentViewport = skIsZero(v) ? true : false;
        break;
    case SK_CONTEXT_SCALE:
        m_options.contextScale.x = skClamp<skScalar>(v, 0.f, 1.f);
        m_options.contextScale.y = m_options.contextScale.x;
        break;
    case SK_Y_UP:
        m_options.yIsUp = skIsZero(v) ? true : false;
        break;
    //case SK_CLEAR_COLOR:
    //case SK_CLEAR_RECT:
    //case SK_CONTEXT_SIZE:
    //case SK_CONTEXT_BIAS:
    //case SK_DEFAULT_FONT:
    //case SK_PROJECTION_TYPE:
    default:
        break;
    }
}

skColor skContext::getContextC(SKcontextOptionEnum op) const
{
    if (op == SK_CLEAR_COLOR)
        return m_options.clearColor;
    return skColor(0, 0, 0, 1);
}

void skContext::setContextC(SKcontextOptionEnum op, const skColor& v)
{
    if (op == SK_CLEAR_COLOR)
        m_options.clearColor = v;
}

skVector2 skContext::getContextV(SKcontextOptionEnum op) const
{
    switch (op)
    {
    case SK_CONTEXT_SIZE:
        return m_options.contextSize;
    case SK_CONTEXT_SCALE:
        return m_options.contextScale;
    case SK_CONTEXT_BIAS:
        return m_options.contextBias;
    //case SK_VERTICES_PER_SEGMENT:
    //case SK_CLEAR_COLOR:
    //case SK_CLEAR_RECT:
    //case SK_OPACITY:
    //case SK_METRICS_MODE:
    //case SK_USE_CURRENT_VIEWPORT:
    //case SK_PROJECTION_TYPE:
    //case SK_Y_UP:
    default:
        break;
    }

    return skVector2(SK_NO_STATUS, SK_NO_STATUS);
}

void skContext::setContextV(SKcontextOptionEnum op, const skVector2& v)
{
    switch (op)
    {
    case SK_CONTEXT_SIZE:
        m_options.contextSize = v;
        break;
    case SK_CONTEXT_SCALE:
        m_options.contextScale = v;
        break;
    case SK_CONTEXT_BIAS:
        m_options.contextBias = v;
        break;
    //case SK_VERTICES_PER_SEGMENT:
    //case SK_CLEAR_COLOR:
    //case SK_CLEAR_RECT:
    //case SK_OPACITY:
    //case SK_METRICS_MODE:
    //case SK_USE_CURRENT_VIEWPORT:
    //case SK_DEFAULT_FONT:
    //case SK_PROJECTION_TYPE:
    //case SK_Y_UP:
    default:
        break;
    }
}

skRectangle skContext::getContextR(SKcontextOptionEnum op) const
{
    if (op == SK_CLEAR_RECT)
        return m_options.clearRectangle;
    return skRectangle(SK_NO_STATUS, SK_NO_STATUS, SK_NO_STATUS, SK_NO_STATUS);
}

void skContext::setContextR(SKcontextOptionEnum op, const skRectangle& v)
{
    switch (op)
    {
    case SK_PROJECTION_TYPE:
        projectRect(v.x, v.y, v.width, v.height);
        break;
    case SK_CLEAR_RECT:
        m_options.clearRectangle = v;
        break;
    //case SK_VERTICES_PER_SEGMENT:
    //case SK_CLEAR_COLOR:
    //case SK_CONTEXT_SIZE:
    //case SK_CONTEXT_SCALE:
    //case SK_CONTEXT_BIAS:
    //case SK_OPACITY:
    //case SK_METRICS_MODE:
    //case SK_USE_CURRENT_VIEWPORT:
    //case SK_DEFAULT_FONT:
    //case SK_Y_UP:
    default:
        break;
    }
}

void skContext::setPaintI(SKpaintStyle op, SKint32 v) const
{
    if (m_workPaint)
        m_workPaint->setI(op, v);
}

SKint32 skContext::getPaintI(SKpaintStyle op) const
{
    SKint32 rValue = SK_NO_STATUS;
    if (m_workPaint)
        m_workPaint->getI(op, &rValue);
    return rValue;
}

void skContext::setPaintF(SKpaintStyle op, SKscalar v) const
{
    if (m_workPaint)
        m_workPaint->setF(op, v);
}

SKscalar skContext::getPaintF(SKpaintStyle op) const
{
    SKscalar rValue = SK_NO_STATUS;
    if (m_workPaint)
        m_workPaint->getF(op, &rValue);
    return rValue;
}

void skContext::setPaintC(SKpaintStyle op, const skColor& v) const
{
    if (m_workPaint)
        m_workPaint->setC(op, v);
}

SKuint32 skContext::getPaintC(SKpaintStyle op) const
{
    SKuint32 rValue = SK_NPOS32;
    if (m_workPaint)
        m_workPaint->getC(op, &rValue);
    return rValue;
}

void skContext::setPaintP(SKpaintStyle op, skTexture* v) const
{
    if (m_workPaint)
        m_workPaint->setT(op, v);
}

skTexture* skContext::getPaintP(SKpaintStyle op) const
{
    if (m_workPaint)
    {
        if (op == SK_BRUSH_PATTERN)
        {
            skTexture* rValue = nullptr;
            m_workPaint->getT(op, &rValue);
            return rValue;
        }
    }
    return nullptr;
}

const SKcontextOptions& skContext::getOptions(void) const
{
    return m_options;
}

skVertexBuffer* skContext::createBuffer() const
{
#ifdef Graphics_BACKEND_OPENGL
    if (m_backend == SK_BE_OpenGL)
        return new skOpenGLVertexBuffer();
#endif
    return nullptr;
}

SKcachedString skContext::newString(void)
{
    if (!m_renderContext)
        return nullptr;

    skCachedString* cStr = new skCachedString();
    cStr->setContext(this);
    cStr->getPath()->setContext(this);

    return (SKcachedString)cStr;
}
