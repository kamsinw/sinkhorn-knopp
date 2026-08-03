cmake_minimum_required(VERSION 3.20)

project(
    Atlas
    VERSION 0.1.0
    LANGUAGES CXX
)

add_library(atlas
    src/matrix/Matrix.cpp
)

add_library(atlas::atlas ALIAS atlas)

target_include_directories(atlas
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(atlas PUBLIC cxx_std_20)
set_target_properties(atlas PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(atlas PRIVATE /W4 /permissive-)
else()
    target_compile_options(atlas PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

include(CTest)

if(BUILD_TESTING)
    add_executable(atlas_matrix_tests
        tests/matrix/MatrixTests.cpp
    )

    target_link_libraries(atlas_matrix_tests PRIVATE atlas::atlas)
    set_target_properties(atlas_matrix_tests PROPERTIES CXX_EXTENSIONS OFF)

    if(MSVC)
        target_compile_options(atlas_matrix_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(
            atlas_matrix_tests
            PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        )
    endif()

    add_test(NAME atlas.matrix COMMAND atlas_matrix_tests)
endif()
