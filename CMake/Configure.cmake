# -----------------------------------------------------------------------------
#
#   Copyright (c) Charles Carley.
#
#   This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
# ------------------------------------------------------------------------------
include(StaticRuntime)
set_static_runtime()

option(Graphics_BUILD_WINDOW      "Compile standalone window system" ON)
option(Graphics_USE_SDL           "Build with SDL" OFF)

if (NOT Graphics_BUILD_WINDOW)
    set(Graphics_USE_SDL OFF CACHE BOOL "Build with SDL" FORCE)
endif()


macro(DefineExternalTarget NAME GROUP PATH)
    set(${NAME}_ExternalTarget      TRUE)
    set(${NAME}_TargetFolders       TRUE)
    set(${NAME}_TargetName          ${NAME})
    set(${NAME}_TargetGroup         ${GROUP})
    set(${NAME}_INCLUDE             ${PATH})
    set(${NAME}_LIBRARY             ${${NAME}_TargetName})
endmacro()

if (Graphics_USE_SDL)
    set(SDL_FOLDER Dependencies)
    set(SDL_LIBRARY SDL2main SDL2-static)
    set(SDL_INCLUDE ${Dependencies_PATH}/SDL/SDL)
    set(Window_WITH_SDL TRUE)
endif()


set(Data2Array_FOLDER TRUE)
set(Data2Array_FOLDER Extern)

set(Dependencies_PATH ${CMAKE_SOURCE_DIR}/Extern)

DefineExternalTarget(Utils         Extern "${Dependencies_PATH}")
DefineExternalTarget(Math          Extern "${Dependencies_PATH}")
DefineExternalTarget(FreeType      Extern "${Dependencies_PATH}/FreeType/Source/2.10.4/include")
DefineExternalTarget(FreeImage     Extern "${Dependencies_PATH}/FreeImage/Source")
DefineExternalTarget(Image         Extern "${Dependencies_PATH}/Image")
DefineExternalTarget(Window        Extern "${Dependencies_PATH}/Window")

find_package(OpenGL REQUIRED)
set(OpenGL_LIB ${OPENGL_LIBRARIES})



