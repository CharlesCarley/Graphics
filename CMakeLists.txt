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
cmake_minimum_required(VERSION 3.0)
project(Graphics)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH 
    ${CMAKE_SOURCE_DIR}/CMake
    ${CMAKE_SOURCE_DIR}/CMake/Modules
    ${CMAKE_SOURCE_DIR}/Extern/Data2Array/CMake
)
include(Configure)
subdirs(CMake)
subdirs(Extern)

configure_file(${CMAKE_SOURCE_DIR}/CMake/skGraphicsConfig.h.in  
               ${CMAKE_BINARY_DIR}/Graphics/skGraphicsConfig.h)

set(Graphics_INCLUDE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${Utils_INCLUDE} 
    ${Math_INCLUDE} 
    ${Image_INCLUDE} 
    ${FreeType_INCLUDE} 
    ${FreeImage_INCLUDE} 
)

set(Graphics_LIBRARY
    Graphics
    ${Utils_LIBRARY} 
    ${Math_LIBRARY} 
    ${Image_LIBRARY} 
    ${FreeType_LIBRARY} 
    ${FreeImage_LIBRARY} 
)

if(Graphics_BACKEND_OPENGL)

    list(APPEND Graphics_INCLUDE
        ${Window_INCLUDE} 
    )
    list(APPEND Graphics_LIBRARY
        ${Window_LIBRARY} 
    )

endif()

subdirs(Graphics)
subdirs(Tests)

