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
#ifndef _FontFragment_
#define _FontFragment_

#include "Graphics/skGraphicsConfig.h"
// clang-format off

SKShader(FontFragment,

precision highp float;

uniform vec4      surface;
uniform sampler2D ima;
varying vec2      texCo;

void main()
{
    float v2 = texture2D(ima, texCo).a;
    if (v2 >= 0.375 && v2 <= 0.7) 
    {
        vec3 v = vec3(1.1) * surface.xyz;
        gl_FragColor = vec4(v.x, v.y, v.z, v2);
    }
    else if (v2 > 0.7)
        gl_FragColor = surface;
    else
        gl_FragColor = vec4(0);
}
);

// clang-format on
#endif  //_FontFragment_