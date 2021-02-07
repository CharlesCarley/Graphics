# -----------------------------------------------------------------------------
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
include(Data2Array)


if (Graphics_ExternalTarget)
    set(TargetFolders ${Graphics_TargetFolders})
    set(TargetName    ${Graphics_TargetName})
    set(TargetGroup   ${Graphics_TargetGroup})
else()
    set(TargetFolders FALSE)
    set(TargetName    Graphics)
    unset(TargetGroup )
endif()

if (NOT Utils_INCLUDE)
    message(SEND_ERROR "Utils_INCLUDE is not defined")
endif()


if (Graphics_BUILD_WINDOW AND Graphics_BACKEND_OPENGL)
    if (NOT Window_INCLUDE)
        message(SEND_ERROR "Window_INCLUDE is not defined")
    endif()
endif()

set(Graphics_INC . ${Graphics_INCLUDE})

set(Graphics_LIB
    ${Utils_LIBRARY}
    ${Math_LIBRARY}
    ${Image_LIBRARY}
    ${FreeType_LIBRARY}
)
  

set(Graphics_HDR
    skCachedString.h
    skContext.h
    skContextObject.h
    skContour.h
    skDefs.h
    skFont.h
    skGlyph.h
    skPaint.h
    skPath.h
    skRender.h
    skTexture.h
    skVertexBuffer.h

    ../skGraphics.h
    ${CMAKE_BINARY_DIR}/Graphics/skGraphicsConfig.h
)

set(Graphics_SRC
    skApi.cpp
    skCachedString.cpp
    skContext.cpp
    skContextObject.cpp
    skFont.cpp
    skGlyph.cpp
    skPaint.cpp
    skPath.cpp
    skTexture.cpp
    skWindowApi.cpp
)


if (Graphics_BACKEND_OPENGL)

    list(APPEND Graphics_SRC
        OpenGL/skCachedProgram.cpp
        OpenGL/skOpenGLRenderer.cpp
        OpenGL/skOpenGLTexture.cpp
        OpenGL/skOpenGLVertexBuffer.cpp
        OpenGL/skProgram.cpp
    )

    list(APPEND Graphics_HDR
        OpenGL/skCachedProgram.h
        OpenGL/skOpenGLRenderer.h
        OpenGL/skOpenGLTexture.h
        OpenGL/skOpenGLVertexBuffer.h
        OpenGL/skProgram.h
    )

    list(APPEND Graphics_LIB
        ${OpenGL_LIB} 
        ${Window_LIBRARY}
    )

    list(APPEND Graphics_INC
        ${Window_INCLUDE}
    )


    if (Graphics_BUILD_WINDOW)
        list(APPEND Graphics_SRC 
            skWindowApi.cpp 
            ../skGraphicsWindow.h)
    endif()

endif()


set(Shaders
    Pipeline/ColoredFragment.inl
    Pipeline/ColoredVertex.inl
    Pipeline/FontFragment.inl
    Pipeline/TexturedFragment.inl
    Pipeline/TexturedVertex.inl
)

set(FONTS 
    Builtin/DejaVuMono.ttf  
    Builtin/DejaVu.ttf
    )

if (Graphics_EXTRA_BUILTIN_FONTS)
    list(APPEND FONTS Builtin/Caveat/Caveat-VariableFont_wght.ttf)
    list(APPEND FONTS Builtin/Rock_Salt/RockSalt-Regular.ttf)
    list(APPEND FONTS Builtin/Roboto/Roboto-Regular.ttf)
    list(APPEND FONTS Builtin/Roboto/Roboto-Light.ttf)
endif()

add_templates_ex(skBuiltinFonts ${FONTS})

source_group(Resource\\Shaders   FILES ${Shaders})
source_group(Resource\\Fonts     FILES ${FONTS})
source_group(Resource\\Generated FILES ${skBuiltinFonts})

include_directories(${Graphics_INC})
add_library(${TargetName} 
    ${Graphics_SRC}
    ${Graphics_HDR}
    ${Shaders}      
    ${FONTS}
    ${skBuiltinFonts}
)
target_link_libraries(${TargetName} ${Graphics_LIB})



set_target_properties(${TargetName} PROPERTIES FOLDER "${TargetGroup}")

