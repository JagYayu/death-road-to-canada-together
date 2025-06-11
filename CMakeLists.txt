cmake_minimum_required(VERSION 3.10)

project(dr2c VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)
add_executable(dr2c ${SRC_FILES})

if (MSVC)
    add_compile_options(/FS /MP /Zc:preprocessor)
endif()

# luajit
find_path(LUAJIT_INCLUDE_DIR luajit.h PATHS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include" PATH_SUFFIXES luajit)
find_library(LUAJIT_LIBRARY NAMES lua51 PATHS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
if(NOT LUAJIT_INCLUDE_DIR OR NOT LUAJIT_LIBRARY)
  message(FATAL_ERROR "LuaJIT not found")
endif()
target_include_directories(dr2c PRIVATE ${LUAJIT_INCLUDE_DIR})
target_link_libraries(dr2c PRIVATE ${LUAJIT_LIBRARY})

# SDL3
set(SDL3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3-3.2.16")
target_include_directories(dr2c PRIVATE "${SDL3_DIR}/include")
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDL3_LIB_DIR "${SDL3_DIR}/lib/x64")
    else()
        set(SDL3_LIB_DIR "${SDL3_DIR}/lib/x86")
    endif()
    target_link_libraries(dr2c PRIVATE "${SDL3_LIB_DIR}/SDL3.lib")
    add_custom_command(TARGET dr2c POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${SDL3_LIB_DIR}/SDL3.dll" "$<TARGET_FILE_DIR:dr2c>")
elseif(UNIX AND NOT APPLE)
    target_link_libraries(dr2c PRIVATE "${SDL3_DIR}/lib/libSDL3.so")
elseif(APPLE)
    target_link_libraries(dr2c PRIVATE "${SDL3_DIR}/lib/libSDL3.dylib")
endif()

# sol2
find_package(sol2 CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE sol2)
