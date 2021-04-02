set(TargetName RandRect)
include_directories(. ${Graphics_INCLUDE})

if(Graphics_WINMAIN)
    add_executable(${TargetName} WIN32 RandRect.cpp index.html)
else()
    add_executable(${TargetName}  RandRect.cpp index.html)
endif()

if(USING_EMSCRIPTEN)
    emscripten_full_es2()
    emscripten_sdl_version(2)
    enable_emscripten_html_executable(${TargetName})
    #emscripten_verbose(${TargetName})
    emscripten_copy_wasm_target_wasm_js(${TargetName} ${CMAKE_CURRENT_SOURCE_DIR}) 
endif()

target_link_libraries(${TargetName}  ${Graphics_LIBRARY})
set_target_properties(${TargetName} PROPERTIES FOLDER "RenderTests")

# file(GLOB Files *.png)
# add_custom_command(
#    TARGET ${TargetName} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy ${Files} ${CMAKE_CURRENT_BINARY_DIR})
