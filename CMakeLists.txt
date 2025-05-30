cmake_minimum_required(VERSION 3.27)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/externals/_vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_TARGET_ARCHITECTURE x64)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(axe_common.cmake)

#set(CMAKE_BUILD_TYPE Debug)

set(PROJECT_NAME axe1)
project(${PROJECT_NAME})
message("==== ${PROJECT_NAME} ====")
#axe_dump_var(VCPKG_TARGET_TRIPLET)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/deploy/_bin/${VCPKG_TARGET_TRIPLET}")
axe_dump_var(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

if (CMAKE_CONFIGURATION_TYPES)
    set(AXE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
else()
    set(AXE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

#axe_dump_cmake_variables()

find_package(fmt                CONFIG REQUIRED)
find_package(EABase             CONFIG REQUIRED)
find_package(EASTL              CONFIG REQUIRED)
find_package(nlohmann_json      CONFIG REQUIRED)
find_package(PNG                       REQUIRED)
find_package(imgui              CONFIG REQUIRED)
find_package(Tracy              CONFIG REQUIRED)

find_package(spirv_cross_core   CONFIG REQUIRED)
find_package(spirv_cross_glsl   CONFIG REQUIRED)
find_package(spirv_cross_hlsl   CONFIG REQUIRED)
find_package(spirv_cross_msl    CONFIG REQUIRED)
find_package(directx-dxc        CONFIG REQUIRED)

#-- core ---
add_subdirectory(src/core)

#-- render ---
add_subdirectory(src/render)

#-- engine ---
add_subdirectory(src/engine)

#-- editor ---
add_subdirectory(src/editor)

#-- tools ---
add_subdirectory(src/tools/editor_app)

#-- test ---
add_subdirectory(src/test/core)
add_subdirectory(src/test/render)
add_subdirectory(src/test/engine)
add_subdirectory(src/test/editor)

#-- externals ---
add_subdirectory(externals)

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT axe_editor_app) # Set As Startup Project