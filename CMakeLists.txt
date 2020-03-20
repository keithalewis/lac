cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(lac VERSION 1.0
            DESCRIPTION "Load And Call C functions"
            LANGUAGES C)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(FFI REQUIRED libffi)
find_package(libffi REQUIRED)

#include(ExternalProject)
#ExternalProject_Add(
#  ffi
#  GIT_REPOSITORY https://github.com/libffi/libffi.git
#  GIT_TAG v3.3
#  CONFIGURE_COMMAND ./autogen.sh && ./configure
#  BUILD_COMMAND make
#  INSTALL_COMMAND make install
#)

set (LAC_SRCS lac_cif.c lac_eval.c lac_init.c lac_map.c lac_token.c lac_variant.c)
set (LAC_TSTS lac_cif.t.c lac_map.t.c lac_token.t.c lac_variant.t.c lac_eval.t.c)
set (LAC_HDRS lac.h lac_cif.h lac_eval.h lac_init.h lac_map.h lac_token.h lac_variant.h)

#add_library(libffi IMPORTED STATIC)
#add_dependencies(libffi ffi)
add_executable(lac lac.c ${LAC_SRCS} ${LAC_HDRS})
#target_include_directories(lac SYSTEM PUBLIC ${FFI_INCLUDE_DIRS})
#target_link_libraries(lac dl ${FFI_LIBRARIES})
target_link_libraries(lac dl ffi)
#target_compile_options(lac PUBLIC ${FFI_CFLAGS_OTHER})

add_executable(lac.t lac.t.c ${LAC_TSTS} ${LAC_SRCS} ${LAC_HDRS})
#target_include_directories(lac.t SYSTEM PUBLIC ${FFI_INCLUDE_DIRS})
target_link_libraries(lac.t dl ffi)
#target_compile_options(lac.t PUBLIC ${FFI_CFLAGS_OTHER})

enable_testing()
add_test(lac.t lac.t)
