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
#pragma once
#include "skGraphicsConfig.h"

SKShader(TexturedVertex,

    attribute vec2 position;
    attribute vec2 textureCoords;
    uniform mat4   viewproj;
    uniform float  zorder;
    varying vec2   texCo;

    void main(void)
    {
        gl_Position = viewproj * vec4(position.x, position.y, zorder, 1);
        texCo       = textureCoords;
    }

);