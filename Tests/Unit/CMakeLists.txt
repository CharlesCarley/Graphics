set(SRC_ALL
    Catch2.h
    catch/catch.hpp
    Main.cpp
    UnitContext.cpp
)


include_directories(. ${Graphics_INCLUDE})
add_executable(GraphicsTest ${SRC_ALL})



target_link_libraries(GraphicsTest ${Graphics_LIBRARY} )
set_target_properties(GraphicsTest  PROPERTIES FOLDER "Units")



add_custom_command(TARGET GraphicsTest 
                PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../Content/test1.png
                "${CMAKE_CURRENT_BINARY_DIR}"
                )

if (Graphics_AUTO_RUN_TESTS)

    add_custom_command(TARGET 
        GraphicsTest POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND  $<TARGET_FILE:GraphicsTest>
    )

endif()

