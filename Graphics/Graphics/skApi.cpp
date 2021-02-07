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
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "Graphics/skGraphics.h"
#include "Math/skBoundingBox2D.h"
#include "skCachedString.h"
#include "skContext.h"
#include "skFont.h"
#include "skPaint.h"
#include "skPath.h"
#include "skTexture.h"

static SKcontext g_currentContext;

/**********************************************************
    Startup / Shutdown
*/

SK_API SKcontext skNewContext()
{
    SKcontext context = (SKcontext) new skContext(SK_BE_OpenGL);
    skSetCurrentContext(context);
    return context;
}

SK_API SKcontext skNewBackEndContext(SKenum backend)
{
    SKcontext context = (SKcontext) new skContext(backend);
    skSetCurrentContext(context);
    return context;
}

SK_API void skDeleteContext(SKcontext ctx)
{
    skContext* context = reinterpret_cast<skContext*>(ctx);
    if (!context)
        return;

    if (ctx == g_currentContext)
        g_currentContext = nullptr;

    delete context;
}

SK_API SKcontext skGetCurrentContext()
{
    return g_currentContext;
}

SK_API void skSetCurrentContext(SKcontext ctx)
{
    if (ctx)
        g_currentContext = ctx;
}

SK_API void skContextSize(SKscalar w, SKscalar h)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setContextV(SK_CONTEXT_SIZE, skVector2(w, h));
}

SK_API void skClearContext()
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->clearContext();
}

SK_API void skSetContext1i(SKcontextOptionEnum en,
                           SKint32             v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setContextI(en, v);
}

SK_API void skSetContext1f(SKcontextOptionEnum en,
                           SKscalar            v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    ctx->setContextF(en, v);
}

SK_API void skGetContext1i(const SKcontextOptionEnum en, SKint32* v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(v, SK_RETURN_VOID);

    *v = ctx->getContextI(en);
}

SK_API void skGetContext1f(SKcontextOptionEnum en, SKscalar* v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(v, SK_RETURN_VOID);

    *v = ctx->getContextF(en);
}

SK_API void skGetContext2i(SKcontextOptionEnum en, SKint32* v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(v, SK_RETURN_VOID);

    skVector2 vec;
    vec  = ctx->getContextV(en);
    *v++ = (SKint32)vec.x;
    *v   = (SKint32)vec.y;
}

SK_API void skGetContext2f(SKcontextOptionEnum en, SKscalar* v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(v, SK_RETURN_VOID);

    skVector2 vec;
    vec  = ctx->getContextV(en);
    *v++ = vec.x;
    *v   = vec.y;
}

SK_API void skSetContext2i(SKcontextOptionEnum en,
                           SKint32             i0,
                           SKint32             i1)
{
    skSetContext2f(en, (SKscalar)i0, (SKscalar)i1);
}

SK_API void skSetContext2f(SKcontextOptionEnum en,
                           SKscalar            f0,
                           SKscalar            f1)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setContextV(en, skVector2(f0, f1));
}

SK_API void skSetContext4i(SKcontextOptionEnum en,
                           SKint32             i0,
                           SKint32             i1,
                           SKint32             i2,
                           SKint32             i3)
{
    skSetContext4f(en, (SKscalar)i0, (SKscalar)i1, (SKscalar)i2, (SKscalar)i3);
}

SK_API void skSetContext4f(SKcontextOptionEnum en,
                           SKscalar            f0,
                           SKscalar            f1,
                           SKscalar            f2,
                           SKscalar            f3)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setContextR(en, skRectangle(f0, f1, f2, f3));
}

SK_API SKpaint skGetWorkingPaint()
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, nullptr);

    return (SKpaint)ctx->getWorkPaint();
}

SK_API SKpath skGetWorkingPath()
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, nullptr);

    return (SKpath)&ctx->getWorkPath();
}

SK_API void skLoadIdentity()
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->getMatrix()
        .makeIdentity();
}

SK_API void skTranslate(SKscalar x, SKscalar y)
{
    skContext* ctx = SK_CURRENT_CTX();

    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    skMatrix4& matrix = ctx->getMatrix();

    skMatrix4 m;
    m.makeIdentity();
    m.setTrans(x, y, 0);
    ctx->getMatrix().multAssign(m, matrix);
}

SK_API void skScale(SKscalar x, SKscalar y)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    skMatrix4& matrix = ctx->getMatrix();

    skMatrix4 m;
    m.makeIdentity();
    m.setScale(x, y, 1);
    ctx->getMatrix().multAssign(m, matrix);
}

SK_API void skRotate(SKscalar r)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    const skQuaternion q(0, 0, r);

    skMatrix4& matrix = ctx->getMatrix();

    skMatrix4 m;
    m.makeIdentity();
    m.makeTransform(skVector3::Zero, skVector3::Unit, q);
    ctx->getMatrix().multAssign(m, matrix);
}

SK_API void skClearColor4f(SKscalar r,
                           SKscalar g,
                           SKscalar b,
                           SKscalar a)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setPaintC(SK_CLEAR_COLOR, skColor(r, g, b, a));
}

SK_API void skClearColor1i(SKcolori rgba)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setContextC(SK_CLEAR_COLOR, skColor(rgba));
}

SK_API void skClear(SKscalar x, SKscalar y, SKscalar w, SKscalar h)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setContextR(SK_CLEAR_RECT, skRectangle(x, y, w, h));
    ctx->clear();
}

SK_API void skProjectContext(SKprojectionType pt)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->projectContext(pt);
}

SK_API void skProjectRect(SKscalar x, SKscalar y, SKscalar w, SKscalar h)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->projectRect(x, y, w, h);
}

SK_API void skProjectBox(SKscalar x1, SKscalar y1, SKscalar x2, SKscalar y2)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->projectBox(x1, y1, x2, y2);
}

SK_API SKpaint skNewPaint()
{
    return (SKpaint) new skPaint();
}

SK_API void skDeletePaint(SKpaint obj)
{
    SK_CHECK_PARAM(obj, SK_RETURN_VOID);

    delete (skPaint*)obj;
}

SK_API void skSelectPaint(SKpaint obj)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->selectPaint((skPaint*)obj);
}

SK_API void skColor1ui(SKuint32 c)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setPaintC(SK_SURFACE_COLOR, skColor(c));
}

SK_API void skColor3f(SKscalar r, SKscalar g, SKscalar b)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setPaintC(SK_SURFACE_COLOR, skColor(r, g, b, skScalar(1.f)));
}

SK_API void skColor4f(SKscalar r, SKscalar g, SKscalar b, SKscalar a)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setPaintC(SK_SURFACE_COLOR, skColor(r, g, b, a));
}

SK_API void skColor3ub(SKubyte r, SKubyte g, SKubyte b)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setPaintC(SK_SURFACE_COLOR,
                   skColor(SKcolorf(r),
                           SKcolorf(g),
                           SKcolorf(b),
                           1.f));
}

SK_API void skColor4ub(SKubyte r, SKubyte g, SKubyte b, SKubyte a)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setPaintC(SK_SURFACE_COLOR,
                   skColor(SKcolorf(r),
                           SKcolorf(g),
                           SKcolorf(b),
                           SKcolorf(a)));
}

SK_API void skSelectImage(SKimage ima)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->selectImage(ima);
}

SK_API void skSetPaint1i(SKpaintStyle en, SKint32 v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setPaintI(en, v);
}

SK_API void skSetPaint1f(SKpaintStyle en, SKscalar v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setPaintF(en, v);
}

SK_API void skSetPaint1ui(SKpaintStyle en, SKuint32 c)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->setPaintC(en, skColor(c));
}

SK_API void skGetPaint1i(SKpaintStyle en, SKint32* v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(v, SK_RETURN_VOID);

    *v = ctx->getPaintI(en);
}

SK_API void skGetPaint1f(SKpaintStyle en, SKscalar* v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(v, SK_RETURN_VOID);

    *v = ctx->getPaintF(en);
}

SK_API void skGetPaint1ui(SKpaintStyle en, SKuint32* v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(v, SK_RETURN_VOID);

    *v = ctx->getPaintC(en);
}

SK_API SKimage skNewImage()
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, nullptr);

    return ctx->newImage();
}

SK_API SKimage skCreateImage(SKuint32 w, SKuint32 h, SKint32 format)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, nullptr);
    SK_CHECK_PARAM(format >= 0 && format < SK_PF_MAX, nullptr);

    return ctx->createImage(w, h, (SKpixelFormat)format);
}

SK_API void skImageLinearGradient(SKimage      ima,
                                  SKcolorStop* stops,
                                  SKint32      stopCount,
                                  SKuint16     dir)
{
    skTexture* img = SKcheckType<skTexture, SKimage, skContext>(ima, SK_CURRENT_CTX());
    SK_CHECK_PARAM(img, SK_RETURN_VOID);
    SK_CHECK_PARAM(stops, SK_RETURN_VOID);

    img->makeLinearGradient(stops, stopCount, dir);
}

SK_API void skImageRadialGradient(SKimage      ima,
                                  SKcolorStop* stops,
                                  SKint32      stopCount)
{
    skTexture* img = SKcheckType<skTexture, SKimage, skContext>(ima, SK_CURRENT_CTX());
    SK_CHECK_PARAM(img, SK_RETURN_VOID);
    SK_CHECK_PARAM(stops, SK_RETURN_VOID);

    img->makeRadialGradient(stops, stopCount);
}

SK_API void skImageLinearGradientEx(SKimage      ima,
                                    SKint32      fx,
                                    SKint32      fy,
                                    SKint32      tx,
                                    SKint32      ty,
                                    SKcolorStop* stops,
                                    SKint32      stopCount)
{
    skTexture* img = SKcheckType<skTexture, SKimage, skContext>(ima, SK_CURRENT_CTX());
    SK_CHECK_PARAM(img, SK_RETURN_VOID);
    SK_CHECK_PARAM(stops, SK_RETURN_VOID);

    img->makeLinearGradient(fx, fy, tx, ty, stops, stopCount);
}

SK_API void skImageRadialGradientEx(SKimage      ima,
                                    SKint32      fx,
                                    SKint32      fy,
                                    SKint32      tx,
                                    SKint32      ty,
                                    SKint32      rx,
                                    SKint32      ry,
                                    SKcolorStop* stops,
                                    SKint32      stopCount)
{
    skTexture* img = SKcheckType<skTexture, SKimage, skContext>(ima, SK_CURRENT_CTX());
    SK_CHECK_PARAM(img, SK_RETURN_VOID);
    SK_CHECK_PARAM(stops, SK_RETURN_VOID);

    img->makeRadialGradient(fx, fy, tx, ty, rx, ry, stops, stopCount);
}

SK_API void skImageSave(SKimage ima, const char* path)
{
    skTexture* img = SKcheckType<skTexture, SKimage, skContext>(ima, SK_CURRENT_CTX());
    SK_CHECK_PARAM(img, SK_RETURN_VOID);
    SK_CHECK_PARAM(path, SK_RETURN_VOID);

    img->save(path);
}

SK_API SKimage skImageLoad(const char* path)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, nullptr);
    SK_CHECK_PARAM(path, nullptr);

    skTexture* tex = (skTexture*)ctx->newImage();
    if (tex != nullptr)
        tex->load(path);
    return (SKimage)tex;
}

SK_API void skDeleteImage(SKimage ima)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->deleteImage(ima);
}

SK_API void skSetImageUV(SKscalar x, SKscalar y, SKscalar w, SKscalar h)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    skPath& pth = ctx->getWorkPath();
    pth.makeUV(x, y, w, h);
}

SK_API void skSetImage1i(SKimage image, SKimageOptionEnum en, SKint32 v)
{
    skTexture* img = SKcheckType<skTexture, SKimage, skContext>(image, SK_CURRENT_CTX());
    SK_CHECK_PARAM(img, SK_RETURN_VOID);

    img->setI(en, v);
}

SK_API void skGetImage1i(SKimage image, SKimageOptionEnum en, SKint32* v)
{
    skTexture* img = SKcheckType<skTexture, SKimage, skContext>(image, SK_CURRENT_CTX());
    SK_CHECK_PARAM(img, SK_RETURN_VOID);

    img->getI(en, v);
}

SK_API void skSetImage1f(SKimage image, SKimageOptionEnum en, SKscalar v)
{
    skTexture* img = SKcheckType<skTexture, SKimage, skContext>(image, SK_CURRENT_CTX());
    SK_CHECK_PARAM(img, SK_RETURN_VOID);

    img->setF(en, v);
}

SK_API void skGetImage1f(SKimage image, SKimageOptionEnum en, SKscalar* v)
{
    skTexture* img = SKcheckType<skTexture, SKimage, skContext>(image, SK_CURRENT_CTX());
    SK_CHECK_PARAM(img, SK_RETURN_VOID);

    img->getF(en, v);
}

SK_API SKfont skNewFont(SKbuiltinFont font, SKuint32 size, SKuint32 dpi)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, nullptr);
    return ctx->newFont(font, size, dpi);
}

SK_API void skSelectFont(SKfont font)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->selectFont(font);
}

SK_API SKfont skNewFontFromFile(const char* path, SKuint32 size, SKuint32 dpi)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, nullptr);
    return ctx->newFontFromFile(path, size, dpi);
}

SK_API void skDeleteFont(SKfont font)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    ctx->deleteFont(font);
}

SK_API SKint32 skGetFontAverageWidth(SKfont font)
{
    skFont* fnt = SKcheckType<skFont, SKfont, skContext>(font, SK_CURRENT_CTX());
    SK_CHECK_PARAM(fnt, 0);

    return fnt->getAverageWidth();
}

SK_API void skGetFontTextExtentEx(SKfont font, const char* text, SKint32 chidx, SKint32 len, SKint32* w, SKint32* h)
{
    skFont* fnt = SKcheckType<skFont, SKfont, skContext>(font, SK_CURRENT_CTX());
    SK_CHECK_PARAM(fnt, SK_RETURN_VOID);
    SK_CHECK_PARAM(text, SK_RETURN_VOID);

    fnt->getTextExtentExt(text, chidx, len, w, h);
}

SK_API void skGetFontTextExtent(SKfont font, const char* text, SKint32 len, SKint32* w, SKint32* h)
{
    skFont* fnt = SKcheckType<skFont, SKfont, skContext>(font, SK_CURRENT_CTX());
    SK_CHECK_PARAM(fnt, SK_RETURN_VOID);
    SK_CHECK_PARAM(text, SK_RETURN_VOID);

    fnt->getTextExtent(text, len, w, h);
}

SK_API void skGetFontCharExtent(SKfont font, char ch, SKint32* w, SKint32* h)
{
    skFont* fnt = SKcheckType<skFont, SKfont, skContext>(font, SK_CURRENT_CTX());
    SK_CHECK_PARAM(fnt, SK_RETURN_VOID);

    fnt->getCharExtent(ch, w, h);
}

SK_API void skGetFontCharExtentEx(SKfont font, char ch, SKtextExtent* te)
{
    skFont* fnt = SKcheckType<skFont, SKfont, skContext>(font, SK_CURRENT_CTX());
    SK_CHECK_PARAM(fnt, SK_RETURN_VOID);

    fnt->getCharExtent(ch, te);
}

SK_API void skDisplayString(SKfont font, const char* str, SKint32 len, SKscalar x, SKscalar y)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(font, SK_RETURN_VOID);
    SK_CHECK_PARAM(str, SK_RETURN_VOID);

    if (len == -1)
        len = (SKint32)strlen(str);

    ctx->displayString(font, str, (SKint32)len, x, y);
}

SK_API void skDisplayFormattedString(SKfont font, SKscalar x, SKscalar y, const char* str, ...)
{
    static char buffer[1025];

    SK_CHECK_PARAM(font, SK_RETURN_VOID);
    SK_CHECK_PARAM(str, SK_RETURN_VOID);

    va_list lst;
    va_start(lst, str);
    int len = vsnprintf(buffer, 1024, str, lst);
    va_end(lst);

    if (len < 0)
        len = 0;
    buffer[len] = 0;

    skDisplayString(font, buffer, len, x, y);
}

SK_API void skSetFont1i(SKfont font, SKfontOptionEnum en, SKint32 v)
{
    skFont* fnt = SKcheckType<skFont, SKfont, skContext>(font, SK_CURRENT_CTX());
    SK_CHECK_PARAM(fnt, SK_RETURN_VOID);

    fnt->setI(en, v);
}

SK_API void skGetFont1i(SKfont font, SKfontOptionEnum en, SKint32* v)
{
    skFont* fnt = SKcheckType<skFont, SKfont, skContext>(font, SK_CURRENT_CTX());
    SK_CHECK_PARAM(fnt, SK_RETURN_VOID);

    fnt->getI(en, v);
}

SK_API void skSetFont1f(SKfont font, SKfontOptionEnum en, SKscalar v)
{
    skFont* fnt = SKcheckType<skFont, SKfont, skContext>(font, SK_CURRENT_CTX());
    SK_CHECK_PARAM(fnt, SK_RETURN_VOID);

    fnt->setF(en, v);
}

SK_API void skGetFont1f(SKfont font, SKfontOptionEnum en, SKscalar* v)
{
    skFont* fnt = SKcheckType<skFont, SKfont, skContext>(font, SK_CURRENT_CTX());
    SK_CHECK_PARAM(fnt, SK_RETURN_VOID);

    fnt->getF(en, v);
}

SK_API SKpath skNewPath()
{
    return (SKpath) new skPath();
}

SK_API void skDeletePath(SKpath pth)
{
    SK_CHECK_PARAM(pth, SK_RETURN_VOID);

    delete (skPath*)pth;
}

SK_API void skSelectPath(SKpath pth)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->selectPath((skPath*)pth);
}

SK_API void skPathSetScale(SKscalar sx, SKscalar sy)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->getWorkPath().setScale(sx, sy);

    ctx->setContextV(SK_CONTEXT_SCALE, skVector2(sx, sy));
}

SK_API void skPathSetBias(SKscalar tx, SKscalar ty)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->getWorkPath()
        .setBias(tx, ty);
    ctx->setContextV(SK_CONTEXT_BIAS, skVector2(tx, ty));
}

SK_API void skPathTransform(SKscalar* matrix)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    // TODO IMPLEMENT
}

SK_API void skGetPathBoundingBox(SKaabbf* bb)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    const skBoundingBox2D& bounds = ctx->getWorkPath().getAabb();

    bb->x1 = bounds.x1;
    bb->y1 = bounds.y1;
    bb->x2 = bounds.x2;
    bb->y2 = bounds.y2;
}

SK_API void skMoveTo(SKscalar x, SKscalar y)
{
    skContext* ctx = SK_CURRENT_CTX();

    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->getWorkPath()
        .moveTo(x, y);
}

SK_API void skLineTo(SKscalar x, SKscalar y)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->getWorkPath()
        .lineTo(x, y);
}

SK_API void skPutVert(SKscalar x, SKscalar y, SKuint8 move)
{
    if (move)
        skMoveTo(x, y);
    else
        skLineTo(x, y);
}

SK_API void skCubicTo(SKscalar fx, SKscalar fy, SKscalar tx, SKscalar ty)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->getWorkPath()
        .cubicTo(fx, fy, tx, ty);
}

SK_API void skRectTo(SKscalar fx, SKscalar fy, SKscalar tx, SKscalar ty)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->getWorkPath()
        .rectTo(fx, fy, tx, ty);
}

SK_API void skArcTo(SKscalar x1, SKscalar y1, SKscalar x2, SKscalar y2, SKscalar angle1, SKscalar angle2, SKwinding winding)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->getWorkPath()
        .arcTo(x1, y1, x2 - x1, y2 - y1, angle1, angle2, winding);
}

SK_API void skArc(SKscalar x, SKscalar y, SKscalar radius, SKscalar angle1, SKscalar angle2, SKwinding winding)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    SKscalar x1, y1, x2, y2;
    x1 = x;
    y1 = y;
    x2 = radius;
    y2 = radius;
    ctx->getWorkPath()
        .arcTo(x1, y1, x2, y2, angle1, angle2, winding);
}

SK_API void skClosePath()
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->getWorkPath()
        .close();
}

SK_API void skClearPath()
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->getWorkPath()
        .clear();
}

SK_API void skRect(SKscalar x, SKscalar y, SKscalar w, SKscalar h)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    skPath& pth = ctx->getWorkPath();
    pth.makeRect(x, y, w, h);
}

SK_API void skLine(SKscalar x1, SKscalar y1, SKscalar x2, SKscalar y2)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    skPath& pth = ctx->getWorkPath();
    pth.clear();
    pth.moveTo(x1, y1);
    pth.lineTo(x2, y2);
}

SK_API void skEllipse(SKscalar x, SKscalar y, SKscalar w, SKscalar h)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    skPath& pth = ctx->getWorkPath();
    pth.makeEllipse(x, y, w, h);
}

SK_API void skRoundRect(SKscalar x, SKscalar y, SKscalar w, SKscalar h, SKscalar aw, SKscalar ah, SKuint16 corners)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    skPath& pth = ctx->getWorkPath();
    pth.makeRoundRect(x, y, w, h, aw, ah, corners);
}

SK_API void skStar(SKscalar x, SKscalar y, SKscalar w, SKscalar h, SKint32 Q, SKint32 P)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    skPath& pth = ctx->getWorkPath();
    pth.makeStar(x, y, w, h, Q, P);
}

SK_API void skPolygon(SKscalar* vertices, SKuint32 count, SKuint32 close)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    skPath& pth = ctx->getWorkPath();
    pth.makePolygon(vertices, count, close != 0);
}

SK_API void skFill()
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->fill();
}

SK_API void skStroke()
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    ctx->stroke();
}

SK_API SKcachedString skNewCachedString()
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, nullptr);

    return ctx->newString();
}

SK_API void skDeleteCachedString(SKcachedString str)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);

    delete SK_CSTRING(str);
}

SK_API void skRebuildCachedString(SKcachedString str)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(str, SK_RETURN_VOID);

    SK_CSTRING(str)->rebuild();
}

SK_API void skBuildCachedString(const SKcachedString str, const char* data)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(str, SK_RETURN_VOID);
    SK_CHECK_PARAM(data, SK_RETURN_VOID);

    skFont* fnt = ctx->getWorkFont();
    SK_CHECK_PARAM(fnt, SK_RETURN_VOID);

    SK_CSTRING(str)->buildString(data);
}

SK_API void skDisplayCachedString(const SKcachedString str)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(str, SK_RETURN_VOID);

    return ctx->displayString(SK_CSTRING(str));
}

SK_API void skGetCachedString2fv(const SKcachedString str, SKstringOptionEnum en, SKscalar* v)
{
    skContext* ctx = SK_CURRENT_CTX();
    SK_CHECK_CTX(ctx, SK_RETURN_VOID);
    SK_CHECK_PARAM(str, SK_RETURN_VOID);
    SK_CHECK_PARAM(v, SK_RETURN_VOID);

    const skVector2 vec = SK_CSTRING(str)->getV(en);
    v[0]                = vec.x;
    v[1]                = vec.y;
}
