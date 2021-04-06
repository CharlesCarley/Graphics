set(TargetName ColorApp)
include_directories(. ${Graphics_INCLUDE})

if(USING_EMSCRIPTEN)
    set(DOC_DIR ${CMAKE_SOURCE_DIR}/docs)

    add_executable(${TargetName}   ${DOC_DIR}/${TargetName}.html ${TargetName}.cpp)

    emscripten_full_es2()
    emscripten_sdl_version(2)
    enable_emscripten_html_executable(${TargetName})
    emscripten_copy_wasm_target_wasm_js(${TargetName} ${DOC_DIR}) 

elseif(Graphics_WINMAIN)
    add_executable(${TargetName} WIN32 ${TargetName}.cpp)
else()
    add_executable(${TargetName}  ${TargetName}.cpp)
endif()

target_link_libraries(${TargetName}  ${Graphics_LIBRARY})
set_target_properties(${TargetName} PROPERTIES FOLDER "RenderTests")
