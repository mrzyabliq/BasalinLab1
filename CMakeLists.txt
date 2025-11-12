cmake_minimum_required(VERSION 3.5)
project(BasalinLab1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


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
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)