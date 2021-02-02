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
#include "skProgram.h"
#include "Utils/skLogger.h"
#include "Window/OpenGL/skOpenGL.h"

static bool SK_OpenGLTestCompile(const char* shaderName, GLuint chk)
{
    GLint ret = 0;
    glGetShaderiv(chk, GL_COMPILE_STATUS, &ret);

    if (ret != GL_TRUE)
    {
        skLogf(LD_ERROR, "GLSL Compile error in: %s\n", shaderName);

        char info[512] = {};

        GLsizei size = 0;
        glGetShaderInfoLog(chk, 511, &size, info);

        skLogd(LD_INFO, info);
        skLogd(LD_INFO, "\n");
    }

    return ret == GL_TRUE;
}

static bool SK_OpenGLTestLink(const char* shaderName, GLuint chk)
{
    GLint ret = 0;
    glGetProgramiv(chk, GL_LINK_STATUS, &ret);

    if (ret != GL_TRUE)
    {
        skLogf(LD_ERROR, "GLSL link error in: %s\n", shaderName);

        char info[512] = {};

        GLsizei size = 0;
        glGetProgramInfoLog(chk, 511, &size, info);

        skLogd(LD_INFO, info);
        skLogd(LD_INFO, "\n");
    }
    return ret == GL_TRUE;
}

skProgram::skProgram() :
    m_program(0)
{
}

skProgram::~skProgram()
{
    if (m_program)
        glDeleteProgram(m_program);

    m_program = 0;
}

bool skProgram::compile(const char* vert,
                        const char* frag,
                        const char* vertName,
                        const char* fragName)
{
    if (!vert || !frag || m_program)
        return false;

    const GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vert, nullptr);
    glCompileShader(vertex);

    if (SK_OpenGLTestCompile(vertName, vertex))
    {
        const GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(fragment, 1, &frag, nullptr);
        glCompileShader(fragment);

        if (SK_OpenGLTestCompile(fragName, fragment))
        {
            m_program = glCreateProgram();
            glAttachShader(m_program, vertex);
            glAttachShader(m_program, fragment);

            glLinkProgram(m_program);
            SK_OpenGLTestLink(vertName, m_program);
        }

        glDeleteShader(fragment);
    }

    glDeleteShader(vertex);

    return true;
}

bool skProgram::compile(const skShaderSource& vertex,
                        const skShaderSource& fragment)
{
    return compile(vertex.source,
                   fragment.source,
                   vertex.file,
                   fragment.file);
}

void skProgram::setUniformMatrix(const char* name, skScalar* matrix) const
{
    if (matrix)
    {
        const GLuint loc = glGetUniformLocation(m_program, name);

        if (loc != (GLuint)-1)
            glUniformMatrix4fv(loc, 1, GL_TRUE, matrix);
    }
}

void skProgram::setUniform4F(const char* name, skScalar* p) const
{
    if (p)
    {
        const GLuint loc = glGetUniformLocation(m_program, name);

        if (loc != (GLuint)-1)
            glUniform4fv(loc, 1, p);
    }
}

void skProgram::setUniform1I(const char* name, SKuint32 i) const
{
    if (m_program)
    {
        const GLuint loc = glGetUniformLocation(m_program, name);

        if (loc != (GLuint)-1)
            glUniform1i(loc, i);
    }
}

void skProgram::setUniform1F(const char* name, skScalar i) const
{
    if (m_program)
    {
        const GLuint loc = glGetUniformLocation(m_program, name);

        if (loc != (GLuint)-1)
            glUniform1f(loc, i);
    }
}

void skProgram::setUniformMatrix(SKuint32 loc, const skScalar* matrix) const
{
    if (m_program && loc != SK_NPOS32)
        glUniformMatrix4fv(loc, 1, GL_TRUE, matrix);
}

void skProgram::setUniform4F(SKuint32 loc, const skScalar* p) const
{
    if (m_program && loc != SK_NPOS32)
        glUniform4fv(loc, 1, p);
}

void skProgram::setUniform1I(SKuint32 loc, SKuint32 i) const
{
    if (m_program && loc != SK_NPOS32)
        glUniform1i(loc, i);
}

void skProgram::setUniform1F(SKuint32 loc, skScalar v) const
{
    if (m_program && loc != SK_NPOS32)
        glUniform1f(loc, v);
}

void skProgram::getUniformLoc(const char* name, SKuint32* d) const
{
    if (d)
    {
        *d = SK_NPOS32;
        if (m_program)
            *d = glGetUniformLocation(m_program, name);
    }
}

void skProgram::bindAttribute(const char* loc, SKuint32 attr) const
{
    if (m_program)
        glBindAttribLocation(m_program, attr, loc);
}

void skProgram::enable(bool val) const
{
    if (m_program)
        glUseProgram(val && m_program ? m_program : 0);
}
