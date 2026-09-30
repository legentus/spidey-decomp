cmake_minimum_required(VERSION 3.20)

project(spidey_input11 LANGUAGES CXX)

add_library(spidey_input11 SHARED
    src/spidey_input11.cpp
    spidey_input11.def
)

add_executable(spidey_input11_probe
    src/spidey_input11_probe.cpp
)

target_include_directories(spidey_input11
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(spidey_input11_probe
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(spidey_input11_probe PRIVATE
    spidey_input11
)

target_compile_features(spidey_input11 PRIVATE cxx_std_17)

if(MSVC)
    target_compile_definitions(spidey_input11 PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    target_compile_options(spidey_input11 PRIVATE
        /W4
        /EHsc
    )
endif()

set_target_properties(spidey_input11 PROPERTIES
    OUTPUT_NAME "spidey_input11"
)
