cmake_minimum_required(VERSION 3.21...3.28)
project(dr2c VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(SDL3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3-3.2.16")
set(SDL3_image_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_image-3.2.4")
set(imgui_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui-1.91.9b")

if (MSVC)
    add_compile_options(/FS /Zc:preprocessor)
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"

	"${imgui_DIR}/*.cpp"
    "${imgui_DIR}/*.c"
    "${imgui_DIR}/*.hpp"
)
add_executable(dr2c ${SRC_FILES})

set_target_properties(dr2c PROPERTIES MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>$<$<CONFIG:Release>:MultiThreadedDLL>")

target_include_directories(dr2c PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ext
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# LuaJIT
find_path(LUAJIT_INCLUDE_DIR luajit.h PATHS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include" PATH_SUFFIXES luajit)
find_library(LUAJIT_LIBRARY NAMES lua51 PATHS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
if (NOT LUAJIT_INCLUDE_DIR OR NOT LUAJIT_LIBRARY)
    message(FATAL_ERROR "LuaJIT not found")
endif()
target_include_directories(dr2c PRIVATE ${LUAJIT_INCLUDE_DIR})
target_link_libraries(dr2c PRIVATE ${LUAJIT_LIBRARY})

# SDL3
target_include_directories(dr2c PRIVATE "${SDL3_DIR}/include")
if (WIN32)
    set(SDL3_LIB_DIR "${SDL3_DIR}/lib/${CMAKE_VS_PLATFORM_NAME}")
    target_link_libraries(dr2c PRIVATE "${SDL3_LIB_DIR}/SDL3.lib")
    add_custom_command(TARGET dr2c POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${SDL3_LIB_DIR}/SDL3.dll" "$<TARGET_FILE_DIR:dr2c>"
    )
elseif(UNIX)
    target_link_libraries(dr2c PRIVATE "${SDL3_DIR}/lib/libSDL3.so")
elseif(APPLE)
    target_link_libraries(dr2c PRIVATE "${SDL3_DIR}/lib/libSDL3.dylib")
endif()

# SDL3_image
target_include_directories(dr2c PRIVATE "${SDL3_image_DIR}/include")
if (WIN32)
    set(SDL3_IMAGE_LIB_DIR "${SDL3_image_DIR}/lib/${CMAKE_VS_PLATFORM_NAME}")
    target_link_libraries(dr2c PRIVATE "${SDL3_IMAGE_LIB_DIR}/SDL3_image.lib")
    add_custom_command(TARGET dr2c POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${SDL3_IMAGE_LIB_DIR}/SDL3_image.dll" "$<TARGET_FILE_DIR:dr2c>"
    )
elseif(UNIX)
    target_link_libraries(dr2c PRIVATE "${SDL3_image_DIR}/lib/libSDL3_image.so")
elseif(APPLE)
    target_link_libraries(dr2c PRIVATE "${SDL3_image_DIR}/lib/libSDL3_image.dylib")
endif()

# imgui
target_include_directories(dr2c PRIVATE ${imgui_DIR})
target_include_directories(dr2c PRIVATE ${imgui_DIR}/backends)

# sol2
find_package(sol2 CONFIG REQUIRED)
target_link_libraries(dr2c PRIVATE sol2)

# glm
target_include_directories(dr2c PRIVATE ${CMAKE_SOURCE_DIR}/lib/glm-master)
