cmake_minimum_required (VERSION 3.20)

project (veeone_filter_demo)

find_package (PkgConfig REQUIRED)


if (WIN32)
    set (OPENGL_LIBRARIES opengl32)
else ()
    pkg_check_modules(OPENGL REQUIRED gl)
endif ()


pkg_check_modules(GLFW REQUIRED glfw3)


option (PADTHV1_STUFF "Implement padthv1 sample view. Requires FFTW3." OFF)

if (PADTHV1_STUFF)
    pkg_check_modules (FFTW3 REQUIRED fftw3f)
    include_directories (${FFTW3_INCLUDE_DIRS})

    set (PADTHV1_SRC rncbc/padthv1_sample.cpp)
    add_compile_definitions("PADTHV1_STUFF=1")
endif ()


set (IMGUI_DIR imgui/)
include_directories (
    .
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    rncbc
)

add_executable (${PROJECT_NAME}
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl2.cpp

    main.cpp
    veeone_filter_widget.cpp
    veeone_adsr_widget.cpp
    veeone_wave_widget.cpp 

    # For wave widget
    rncbc/synthv1_wave.cpp
    ${PADTHV1_SRC}
)

target_link_libraries (${PROJECT_NAME} PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${FFTW3_LIBRARIES}
)
