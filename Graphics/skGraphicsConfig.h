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
#ifndef _skGraphicsConfig_h_
#define _skGraphicsConfig_h_

#define Graphics_BUILD_WINDOW
/* #undef Graphics_NO_PALETTE */
#define Graphics_OP_CHECKS 
#define Graphics_EXTRA_BUILTIN_FONTS


#define SK_CAST(x, T) reinterpret_cast<T>(x)
#define SK_CAST_CTX(x) SK_CAST(x, skContext*)
#define SK_CURRENT_CTX() SK_CAST(g_currentContext, skContext*)
#define SK_GET_WINDOW_MGR(mgr) SK_CAST(mgr, skWindowManager*)
#define SK_GET_WINDOW(win) SK_CAST(win, skWindow*)

#define SK_RETURN_VOID
#define SKcolorf(x) (skScalar(x) * skColorUtils::i255)

#ifndef Graphics_OP_CHECKS

#define SK_CHECK_PARAM(x, r)
#define SK_CHECK_CTX(x, r)

#else

#define SK_CHECK_PARAM(x, r) \
    SK_ASSERT(x);            \
    if (!(x))                \
        return r

#define SK_CHECK_CTX(x, r) SK_CHECK_PARAM(((x) && (x)->isValid()), r)

#endif


typedef struct skShaderSource
{
    const char* source;
    const char* file;
} skShaderSource;

#define SKStringify(NAME, x) static const char* NAME = #x

#define SKShader(NAME, x) static const skShaderSource NAME = { \
                              #x, __FILE__}


#endif  //_skGraphicsConfig_h_
