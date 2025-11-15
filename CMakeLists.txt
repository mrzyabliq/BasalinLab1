cmake_minimum_required(VERSION 3.5)
project(BasalinLab1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE "GLVND")
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)
set(IMGUI_DIR_BACK ${CMAKE_SOURCE_DIR}/imgui/backends)
set(FONT_PATH ${CMAKE_SOURCE_DIR}/Frontend/imgui/misc/fonts/DroidSans.ttf)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

if(NOT OpenGL_FOUND OR NOT glfw3_FOUND)
   message(FATAL_ERROR "Необходимые библиотеки (OpenGL и GLFW) не найдены.")
endif()

find_package(Eigen3 3.3 QUIET)

if(NOT Eigen3_FOUND)
    include(FetchContent)
    message(STATUS "Eigen3 not found — FetchContent will download Eigen")
    FetchContent_Declare(
        Eigen3
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
    )
    FetchContent_MakeAvailable(Eigen3)
endif()

set(SOURCES
    src/impls/Matrix.cpp
    src/impls/VMatrix.cpp
    src/impls/Graph.cpp
    src/impls/JsonReader.cpp
    src/impls/Plot.cpp
    src/main.cpp
)

set(HEADERS
    src/headers/Matrix.h
    src/headers/EulerSolver.h
    src/headers/VMatrix.h
    src/headers/Component.h
    src/headers/Circuit.h
    src/headers/Graph.h
    src/headers/JsonReader.h
    src/headers/Plot.h
)

add_definitions(-DFONT_PATH=\"${FONT_PATH}\")

include_directories(
    ${IMGUI_DIR}
    ${IMGUI_DIR_BACK}
)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/implot.cpp
    ${IMGUI_DIR}/implot_items.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Eigen3::Eigen
        imgui
        glfw
        ${OPENGL_LIBRARIES}
        GL
)

target_compile_definitions(imgui PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)