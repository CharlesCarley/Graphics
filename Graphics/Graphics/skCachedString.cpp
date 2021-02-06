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
#include "skCachedString.h"
#include "OpenGL/skOpenGLTexture.h"
#include "Utils/skLogger.h"
#include "skContext.h"
#include "skFont.h"

skCachedString::skCachedString() :
    m_cache()
{
    m_path = new skPath();
    m_size = skVector2::Zero;
}

skCachedString::~skCachedString()
{
    if (m_path)
    {
        delete m_path;
        m_path = nullptr;
    }
}

void skCachedString::rebuild()
{
    if (m_cache.empty())
        return;

    const skContext& ctx   = ref();
    const skVector2  scale = ctx.getContextV(SK_CONTEXT_SCALE);

    skFont* fnt = ctx.getWorkFont();
    if (!fnt)
    {
        skLogd(LD_ERROR, "rebuild string called without a selected font\n");
        return;
    }

    m_path->setContext(this->getContext());
    m_path->clear();

    fnt->buildPath(m_path, m_cache.c_str(), (SKuint32)m_cache.size(), 0, 0);

    SKint32 w, h;
    fnt->getTextExtent(m_cache.c_str(), (SKuint32)m_cache.size(), &w, &h);
    m_size.x = (skScalar)w;
    m_size.y = (skScalar)h;
}


void skCachedString::buildString(const char* str)
{
    const skContext& ctx   = ref();
    const skVector2  scale = ctx.getContextV(SK_CONTEXT_SCALE);

    skFont* fnt = ctx.getWorkFont();
    if (!fnt)
    {
        skLogd(LD_ERROR, "build string called without a selected font\n");
        return;
    }

    m_cache.clear();
    m_cache = str;

    m_path->setContext(this->getContext());
    m_path->clear();

    fnt->buildPath(m_path, m_cache.c_str(), (SKuint32)m_cache.size(), 0, 0);

    SKint32 w, h;
    fnt->getTextExtent(m_cache.c_str(), (SKuint32)m_cache.size(), &w, &h);
    m_size.x = (skScalar)w;
    m_size.y = (skScalar)h;
}

skVector2 skCachedString::getV(SKstringOptionEnum op) const
{
    switch (op)
    {
    case SK_STRING_SIZE:
    case SK_STRING_EXTENT:
        return m_size;
    }
    return skVector2(SK_NO_STATUS, SK_NO_STATUS);
}
