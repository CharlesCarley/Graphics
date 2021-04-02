set(TargetName CTest)
include_directories(. ${Graphics_INCLUDE})

if(Graphics_WINMAIN)
    add_executable(${TargetName} WIN32 CTest.c)
else()
    add_executable(${TargetName}  CTest.c)
endif()

target_link_libraries(${TargetName}  ${Graphics_LIBRARY})
set_target_properties(${TargetName} PROPERTIES FOLDER "RenderTests")

file(GLOB Files *.png)

add_custom_command(
    TARGET ${TargetName} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${Files} ${CMAKE_CURRENT_BINARY_DIR})
