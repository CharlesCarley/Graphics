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
#ifndef _skContext_h_
#define _skContext_h_

#include "skContextObject.h"

class skContext
{
private:
    skRenderer*      m_renderContext;
    SKint32          m_id;
    skPaint*         m_workPaint;
    skPaint*         m_tempPaint;
    skPath*          m_workPath;
    skFont*          m_workFont;
    skPath*          m_tempPath;
    SKint32          m_backend;
    skMatrix4        m_matrix;
    SKcontextOptions m_options;

public:
    skContext(SKint32 backend);
    ~skContext();

    void makeCurrent(skRenderer* ctx);

    void makeRect(const skRectangle& r) const;

    void makeRect(const skScalar& x, const skScalar& y, const skScalar& w, const skScalar& h) const;

    void projectContext(SKprojectionType pt);

    void projectRect(skScalar x, skScalar y, skScalar w, skScalar h) const;

    void projectBox(skScalar x1, skScalar y1, skScalar x2, skScalar y2) const;

    void clearContext(void) const;

    void clear(void) const;

    void fill(void) const;

    void stroke(void) const;

    SKimage createImage(SKuint32 w, SKuint32 h, SKpixelFormat fmt);

    SKimage newImage();

    skTexture* createInternalImage(SKuint32 w, SKuint32 h, SKpixelFormat fmt);

    void deleteImage(SKimage ima);

    void selectImage(SKimage ima);

    SKfont newFont(SKbuiltinFont font, SKuint32 size, SKuint32 dpi);

    void selectFont(SKfont font);

    SKfont newFontFromFile(const char* path, SKuint32 size, SKuint32 dpi);

    SKfont newFontFromMemory(const void* mem, SKuint32 len, SKuint32 size, SKuint32 dpi);

    void deleteFont(SKfont font);

    SKcachedString newString(void);

    void displayString(skCachedString* str) const;

    void displayString(SKfont font, const char* str, SKuint32 len, skScalar x, skScalar y);

    void selectPath(skPath* pth);

    void selectPaint(skPaint* obj);

    SKint32 getContextI(SKcontextOptionEnum opt) const;

    void setContextI(SKcontextOptionEnum opt, SKint32 v);

    skScalar getContextF(SKcontextOptionEnum opt) const;

    void setContextF(SKcontextOptionEnum opt, skScalar v);

    skColor getContextC(SKcontextOptionEnum opt) const;

    void setContextC(SKcontextOptionEnum opt, const skColor& v);

    skRectangle getContextR(SKcontextOptionEnum opt) const;

    void setContextR(SKcontextOptionEnum opt, const skRectangle& v);

    skVector2 getContextV(SKcontextOptionEnum opt) const;

    void setContextV(SKcontextOptionEnum opt, const skVector2& v);

    void setPaintI(SKpaintStyle op, SKint32 v) const;

    SKint32 getPaintI(SKpaintStyle op) const;

    void setPaintF(SKpaintStyle op, SKscalar v) const;

    SKscalar getPaintF(SKpaintStyle op) const;

    void setPaintP(SKpaintStyle op, skTexture* v) const;

    skTexture* getPaintP(SKpaintStyle op) const;

    void setPaintC(SKpaintStyle op, const skColor& v) const;

    SKuint32 getPaintC(SKpaintStyle op) const;

    const SKcontextOptions& getOptions(void) const;

    skVertexBuffer* createBuffer();

    SKint32 getId(void) const
    {
        return m_id;
    }

    const skPaint& getWorkPaint(void) const
    {
        SK_ASSERT(m_workPaint);
        return *m_workPaint;
    }

    const skPath& getWorkPath(void) const
    {
        SK_ASSERT(m_workPath);
        return *m_workPath;
    }

    skPaint* getWorkPaint(void)
    {
        return m_workPaint;
    }

    skPath& getWorkPath(void)
    {
        SK_ASSERT(m_workPath);
        return *m_workPath;
    }

    skFont* getWorkFont(void) const
    {
        // must check, this is allowed to be null
        return m_workFont;
    }

    skMatrix4& getMatrix(void)
    {
        return m_matrix;
    }

    const skMatrix4& getMatrix(void) const
    {
        return m_matrix;
    }

    skRenderer* getCurrent(void) const
    {
        return m_renderContext;
    }

    bool isValid(void) const
    {
        return m_backend == SK_BE_None || m_renderContext != nullptr;
    }
};

#endif  //_skContext_h_
