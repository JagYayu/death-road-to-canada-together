cmake_minimum_required(VERSION 3.21...3.28)
project(dr2c VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

if(MSVC)
    add_compile_options(/FS /Zc:preprocessor)
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)
add_executable(dr2c ${SRC_FILES})

set_target_properties(dr2c PROPERTIES
    MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>$<$<CONFIG:Release>:MultiThreadedDLL>"
)

target_include_directories(dr2c PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ext
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# EASTL - from vcpkg
find_package(EASTL CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE EASTL)

# LuaJIT - from vcpkg dir
find_path(LUAJIT_INCLUDE_DIR luajit.h PATHS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include" PATH_SUFFIXES luajit)
find_library(LUAJIT_LIBRARY NAMES lua51 PATHS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
if(NOT LUAJIT_INCLUDE_DIR OR NOT LUAJIT_LIBRARY)
    message(FATAL_ERROR "LuaJIT not found")
endif()
target_include_directories(dr2c PRIVATE ${LUAJIT_INCLUDE_DIR})
target_link_libraries(dr2c PRIVATE ${LUAJIT_LIBRARY})

# sol2 - from vcpkg
find_package(sol2 CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE sol2::sol2)

# SDL3
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE SDL3::SDL3)

# SDL3_image
find_package(SDL3_image CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)

# SDL3_ttf
find_package(SDL3_ttf CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE SDL3_ttf::SDL3_ttf)

# enet
find_package(unofficial-enet CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE unofficial::enet::enet)
target_link_libraries(dr2c PRIVATE ws2_32)
target_link_libraries(dr2c PRIVATE winmm)

# imgui - from vcpkg
find_package(imgui CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE imgui::imgui)

# glm - from thirdparty lib
target_include_directories(dr2c PRIVATE ${CMAKE_SOURCE_DIR}/lib/glm-master)

# stb - from vcpkg
find_package(Stb REQUIRED)
target_include_directories(dr2c PRIVATE ${Stb_INCLUDE_DIR})
