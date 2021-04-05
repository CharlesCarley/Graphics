set(TargetName CTest)
include_directories(. ${Graphics_INCLUDE})

if(USING_EMSCRIPTEN)
    add_executable(${TargetName}   ${TargetName}.html ${TargetName}.c)

    emscripten_full_es2()
    emscripten_sdl_version(2)
    enable_emscripten_html_executable(${TargetName})
    emscripten_copy_wasm_target_wasm_js(${TargetName} ${CMAKE_CURRENT_SOURCE_DIR}) 

elseif(Graphics_WINMAIN)
    add_executable(${TargetName} WIN32 ${TargetName}.c)
else()
    add_executable(${TargetName}  ${TargetName}.c)
endif()

target_link_libraries(${TargetName}  ${Graphics_LIBRARY})
set_target_properties(${TargetName} PROPERTIES FOLDER "RenderTests")

file(GLOB Files *.png)

add_custom_command(
    TARGET ${TargetName} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${Files} ${CMAKE_CURRENT_BINARY_DIR})
