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

SKShader(TexturedFragment,

    uniform vec4      surface;
    uniform sampler2D ima;
    uniform int       mode;
    varying vec2      texCo;

    vec4 sk_replace(void)
    {
        vec4 col = texture2D(ima, texCo);

        col.a *= surface.a;
        if (col.a > 1.0)
            col.a = 1.0;
        if (col.a < 0.0)
            col.a = 0.0;
        return col;
    }

    vec4 sk_modulate(void)
    {
        return texture2D(ima, texCo) * surface;
    }

    vec4 sk_add(void)
    {
        return texture2D(ima, texCo) + surface;
    }

    vec4 sk_sub(void)
    {
        return texture2D(ima, texCo) - surface;
    }

    void main(void)
    {
        if (mode == 4)  // SK_BM_SUBTRACT
            gl_FragColor = sk_sub();
        else if (mode == 2)  // SK_BM_ADD
            gl_FragColor = sk_add();
        else if (mode == 3)  // SK_BM_MODULATE
            gl_FragColor = sk_modulate();
        else
            gl_FragColor = sk_replace();
    }

);
