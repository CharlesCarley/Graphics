if(Graphics_BACKEND_OPENGL AND Graphics_BUILD_RENDER_TESTS)
    subdirs(RandRect)
    subdirs(ColorApp)
    subdirs(CTest)
endif()

subdirs(Unit)