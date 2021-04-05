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
#include "OpenGL/skOpenGLTexture.h"
#include "Utils/Config/skConfig.h"
#include "Utils/skDisableWarnings.h"
#include "Window/Window/OpenGL/skOpenGL.h"
#include "skContext.h"

void SK_GetFormat(int fmt, GLenum& glfmt)
{
    glfmt = GL_RGB;
    switch (fmt)
    {
    case 2:
    case 1:
        glfmt = GL_ALPHA;
        break;
    case 3:
        glfmt = GL_RGB;
        break;
    case 4:
#if SK_PLATFORM == SK_PLATFORM_EMSCRIPTEN
        glfmt = GL_RGBA;
#else
        glfmt = GL_BGRA;
#endif
    default:;
    }
}

void SK_GetMinMag(const SKuint32& filter, GLint& min, GLint& mag, bool mipmap)
{
    switch (filter)
    {
    default:
    case SK_FILTER_NONE:
        mag = min = mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
        break;
    case SK_FILTER_NONE_LINEAR:
        min = mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
        mag = mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        break;
    case SK_FILTER_LINEAR_NONE:
        min = mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        mag = mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
        break;
    case SK_FILTER_BI_LINEAR:
        min = mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        mag = mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        break;
    }
}

skOpenGLTexture::skOpenGLTexture() :
    skTexture(), m_dirty(true), m_tex(0)
{
}

skOpenGLTexture::skOpenGLTexture(SKint32 w, SKint32 h, SKpixelFormat fmt) :
    skTexture(w, h, fmt),
    m_dirty(true),
    m_tex(0)
{
}

skOpenGLTexture::~skOpenGLTexture()
{
    glDeleteTextures(1, &m_tex);
}

SKuint32 skOpenGLTexture::getImage(void)
{
    if (m_dirty && m_image)
    {
        m_dirty = false;

        GLenum format;
        GLint  min, mag;

        SK_GetFormat(m_image->getBPP(), format);
        SK_GetMinMag(m_opts.filter, min, mag, m_opts.mipmap != 0);

        glGenTextures(1, &m_tex);
        glBindTexture(GL_TEXTURE_2D, m_tex);
        glEnable(GL_TEXTURE_2D);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     m_image->getWidth(),
                     m_image->getHeight(),
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     m_image->getBytes());

        if (m_opts.mipmap)
            glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
        glDisable(GL_TEXTURE_2D);
    }
    return m_tex;
}
