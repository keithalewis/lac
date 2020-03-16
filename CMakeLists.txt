cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(lac VERSION 1.0
            DESCRIPTION "Load And Call C functions"
            LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFI REQUIRED libffi)

add_executable(lac ensure.h lac.c lac.h lac_cif.c lac_cif.h lac_eval.c
lac_eval.h lac_init.c lac_init.h lac_map.c lac_map.h lac_token.c
lac_token.h lac_variant.c lac_variant.h)

target_include_directories(lac SYSTEM PUBLIC ${FFI_INCLUDE_DIRS})
target_link_libraries(lac ${FFI_LIBRARIES})
target_compile_options(lac PUBLIC ${FFI_CFLAGS_OTHER})
