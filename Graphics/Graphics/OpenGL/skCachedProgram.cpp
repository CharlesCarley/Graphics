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
#include "skCachedProgram.h"

skCachedProgram::skCachedProgram() :
    m_zOrder(SK_MAX32),
    m_viewProj(SK_MAX32),
    m_mode(SK_MAX32),
    m_surface(SK_MAX32),
    m_brush(SK_MAX32),
    m_ima(SK_MAX32)
{
}

skCachedProgram::~skCachedProgram()
{
}

void skCachedProgram::setZOrder(skScalar z)
{
    if (m_zOrder == SK_MAX32)
        this->getUniformLoc("zorder", &m_zOrder);

    if (m_zOrder != SK_NPOS32)
        setUniform1F(m_zOrder, z);
}

void skCachedProgram::setViewProj(const skMatrix4& vProj)
{
    if (m_viewProj == SK_MAX32)
        this->getUniformLoc("viewproj", &m_viewProj);

    if (m_viewProj != SK_NPOS32)
        setUniformMatrix(m_viewProj, vProj.p);
}

void skCachedProgram::setImage(SKuint32 ima)
{
    if (m_ima == SK_MAX32)
        this->getUniformLoc("ima", &m_ima);

    if (m_ima != SK_NPOS32)
        setUniform1I(m_ima, ima);
}

void skCachedProgram::setMode(SKuint32 m)
{
    if (m_mode == SK_MAX32)
        this->getUniformLoc("mode", &m_mode);

    if (m_mode != SK_NPOS32)
        setUniform1I(m_mode, m);
}

void skCachedProgram::setSurface(const skScalar* p)
{
    if (m_surface == SK_MAX32)
        this->getUniformLoc("surface", &m_surface);

    if (m_surface != SK_NPOS32)
        setUniform4F(m_surface, p);
}

void skCachedProgram::setBrush(const skScalar* p)
{
    if (m_brush == SK_MAX32)
        this->getUniformLoc("brush", &m_brush);

    if (m_brush != SK_NPOS32)
        setUniform4F(m_brush, p);
}
