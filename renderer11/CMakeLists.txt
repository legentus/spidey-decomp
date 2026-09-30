cmake_minimum_required(VERSION 3.20)

project(spidey_renderer11 LANGUAGES CXX)

add_library(spidey_renderer11 SHARED
    src/spidey_renderer11.cpp
)

target_include_directories(spidey_renderer11
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(spidey_renderer11 PRIVATE cxx_std_17)

if(MSVC)
    target_compile_definitions(spidey_renderer11 PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    target_compile_options(spidey_renderer11 PRIVATE
        /W4
        /EHsc
    )
endif()

target_link_libraries(spidey_renderer11 PRIVATE
    d3d11
    dxgi
)

set_target_properties(spidey_renderer11 PROPERTIES
    OUTPUT_NAME "spidey_renderer11"
)
