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
#include "Graphics/skGraphicsConfig.h"


// 1, SK_BM_REPLACE,
// 2, SK_BM_ADD,
// 3, SK_BM_MODULATE,
// 4, SK_BM_SUBTRACT,
// 5, SK_BM_DIVIDE,

SKShader(TexturedFragment,
    precision highp float;

    uniform vec4      surface;
    uniform sampler2D ima;
    uniform int       mode;
    varying vec2      texCo;

    void main()
    {
        vec4 img = texture2D(ima, texCo);
        if (mode == 1)  // SK_BM_REPLACE
        {
            gl_FragColor = img;
        }
        else if (mode == 2) // SK_BM_ADD
        {
            vec3 obj     = img.xyz + surface.xyz;
            gl_FragColor = vec4(obj.x, obj.y, obj.z, img.a * surface.a);
        }
        else if (mode == 3)  // SK_BM_MODULATE
        {
            vec3 obj     = surface.xyz * img.xyz;
            gl_FragColor = vec4(obj.x, obj.y, obj.z, img.a * surface.a);
        }
        else if (mode == 4)  // SK_BM_SUBTRACT
        {
            vec3 obj     = surface.xyz - img.xyz;
            gl_FragColor = vec4(obj.x, obj.y, obj.z, img.a * surface.a);
        }
        else // SK_BM_DIVIDE
        {
            vec3 obj = vec3(1.0) - (surface.xyz * img.xyz);
            gl_FragColor = vec4(obj.x, obj.y, obj.z, img.a * surface.a);
        }
    }

);
