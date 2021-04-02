if(Graphics_BACKEND_OPENGL AND Graphics_BUILD_RENDER_TESTS)
    subdirs(RandRect)
    subdirs(ColorApp)
    subdirs(CTest)
endif()


if (NOT USING_EMSCRIPTEN)
   subdirs(Unit)
endif()