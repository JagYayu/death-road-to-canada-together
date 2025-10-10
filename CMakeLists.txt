cmake_minimum_required(VERSION 3.21...3.28)

project(tudov
    VERSION 1.0
    LANGUAGES CXX
    DESCRIPTION "Tudov Engine - Death Road to Canada Together"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

if(MSVC)
    add_compile_options(/bigobj /FS /Zc:preprocessor)
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Lib/imgui-1.91.9b/*.cpp"
)
add_executable(tudov ${SRC_FILES})

target_include_directories(tudov PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Lib/imgui-1.91.9b
    ${CMAKE_CURRENT_SOURCE_DIR}/Lib/imgui-1.91.9b/backends
)

# LuaJIT - from vcpkg dir
find_path(LUAJIT_INCLUDE_DIR luajit.h PATHS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include" PATH_SUFFIXES luajit)
find_library(LUAJIT_LIBRARY NAMES lua51 PATHS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
if(NOT LUAJIT_INCLUDE_DIR OR NOT LUAJIT_LIBRARY)
    message(FATAL_ERROR "LuaJIT not found")
endif()
target_include_directories(tudov PRIVATE ${LUAJIT_INCLUDE_DIR})
target_link_libraries(tudov PRIVATE ${LUAJIT_LIBRARY})

# sol2 - from vcpkg
find_package(sol2 CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE sol2::sol2)

# SDL3
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE SDL3::SDL3)

# SDL3_image
find_package(SDL3_image CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)

# SDL3_ttf
find_package(SDL3_ttf CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE SDL3_ttf::SDL3_ttf)

# OpenAL
find_package(OpenAL CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE OpenAL::OpenAL)

# enet
find_package(unofficial-enet CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE unofficial::enet::enet)
target_link_libraries(tudov PRIVATE ws2_32)
target_link_libraries(tudov PRIVATE winmm)

# minizip - from vcpkg
find_package(unofficial-minizip CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE unofficial::minizip::minizip)

# bitsery
find_package(Bitsery CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE Bitsery::bitsery)
