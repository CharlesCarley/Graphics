

macro(AddTestExt File Ext)
   include_directories(. ../Graphics/ 
        ${Utils_INCLUDE} 
        ${Math_INCLUDE} 
        ${Window_INCLUDE} 
        ${Image_INCLUDE} 
        ${FreeType_INCLUDE} 
        ${FreeImage_INCLUDE} 
        )

   add_executable(${File}  ${File}.${Ext} ${ARGN})
   target_link_libraries(${File} 
        Graphics 
        ${Math_LIBRARY}
        ${Utils_LIBRARY}
        ${Window_LIBRARY}
        ${Image_LIBRARY}
        ${FreeImage_LIBRARY}
        )
   set_target_properties(${File} PROPERTIES FOLDER "Tests")

   file(GLOB Files *.png)

    add_custom_command(TARGET ${File} 
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy ${Files}
                    "${CMAKE_CURRENT_BINARY_DIR}"
                    )

endmacro()


macro(AddTest File)
    AddTestExt(${File} "cpp")
endmacro()

macro(AddCTest File)
    AddTestExt(${File} "c")
endmacro()

AddTest(RandRect)
AddTest(MiscTest)
AddTest(ColorApp)
AddCTest(CTest)





set(SRC_ALL
    Catch2.h
    catch/catch.hpp
    Main.cpp
    UnitContext.cpp
)


include_directories(. ../
    ${Utils_INCLUDE} 
    ${Math_INCLUDE} 
    ${Window_INCLUDE} 
    ${Image_INCLUDE} 
    ${FreeType_INCLUDE} 
    ${FreeImage_INCLUDE} 
    )

add_executable(UnitTests ${SRC_ALL})
target_link_libraries(UnitTests
    Graphics 
    ${Math_LIBRARY}
    ${Utils_LIBRARY}
    ${Window_LIBRARY}
    ${Image_LIBRARY}
    ${FreeImage_LIBRARY}
    )
set_target_properties(UnitTests  PROPERTIES FOLDER "Tests")
