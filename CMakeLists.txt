cmake_minimum_required(VERSION 3.15)

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
    add_compile_options(/FS /MP)
endif()

# bgfx
find_package(bgfx CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE bgfx::bx bgfx::bgfx bgfx::bimg bgfx::bimg_decode)

# luajit
find_path(LUAJIT_INCLUDE_DIR luajit.h PATHS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include" PATH_SUFFIXES luajit)
find_library(LUAJIT_LIBRARY NAMES lua51 PATHS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
if(NOT LUAJIT_INCLUDE_DIR OR NOT LUAJIT_LIBRARY)
  message(FATAL_ERROR "LuaJIT not found")
endif()
target_include_directories(dr2c PRIVATE ${LUAJIT_INCLUDE_DIR})
target_link_libraries(dr2c PRIVATE ${LUAJIT_LIBRARY})

# SDL2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(dr2c
	PRIVATE
	$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
	$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

# sol2
find_package(sol2 CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE sol2)
