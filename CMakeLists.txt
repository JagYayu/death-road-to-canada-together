cmake_minimum_required(VERSION 3.21...3.28)

project(tudov
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Death Road To Canada"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

if(MSVC)
    add_compile_options(/FS /Zc:preprocessor)
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
add_executable(tudov ${SRC_FILES})

set_target_properties(tudov PROPERTIES
    MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>$<$<CONFIG:Release>:MultiThreadedDLL>"
)

target_include_directories(tudov PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ext
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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

# enet
find_package(unofficial-enet CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE unofficial::enet::enet)
target_link_libraries(tudov PRIVATE ws2_32)
target_link_libraries(tudov PRIVATE winmm)

# imgui - from vcpkg
find_package(imgui CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE imgui::imgui)

# minizip - from vcpkg
find_package(unofficial-minizip CONFIG REQUIRED)
target_link_libraries(tudov PRIVATE unofficial::minizip::minizip)
