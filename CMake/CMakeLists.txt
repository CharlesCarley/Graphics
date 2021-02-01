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

if (MSVC)

    set(FilePatterns
        CMake:*.cmake
        CMake:../CMakeLists.txt
        Include:*.in
        BatchScript:*.bat
        BatchScript:*.sh
        Configuration:*.config
        Configuration:*.conf
        Configuration:*.cfg
        Configuration:*.ini
        Json:*.json
        Python:*.py
        CLang:../*.clang-format
        GitHub:../.github/workflows/*.yml
    )
    set(ProjectFiles )

    foreach(PatternString ${FilePatterns})
        string(FIND       ${PatternString} ":" SplitIdx)
        string(LENGTH     ${PatternString}  Strlen)
        string(SUBSTRING  ${PatternString} 0 ${SplitIdx} Group)
        string(SUBSTRING  ${PatternString} ${SplitIdx} ${Strlen} Pattern)
        string(REPLACE    ":" "" Pattern ${Pattern})

        file(GLOB PatternFiles ${Pattern})

        if (PatternFiles)
            source_group(${Group} FILES ${PatternFiles})
            list(APPEND ProjectFiles ${PatternFiles})
        endif()
    endforeach()

    set(CMAKE_FILES ${ProjectFiles})
    set_source_files_properties(${ProjectFiles} PROPERTIES VS_TOOL_OVERRIDE "None")

    add_custom_target(ALL_CMAKE ALL SOURCES ${ProjectFiles})
    set_target_properties(ALL_CMAKE PROPERTIES FOLDER "CMakePredefinedTargets")

endif()
