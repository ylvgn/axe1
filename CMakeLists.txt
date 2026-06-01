cmake_minimum_required(VERSION 3.27) # Available use vs2026 since 3.27+

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/externals/_vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_TARGET_ARCHITECTURE x64)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(axe_common.cmake)

#set(CMAKE_BUILD_TYPE Debug)
set(AXE_PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(PROJECT_NAME "axe1")

if (CMAKE_GENERATOR_TOOLSET)
	set(PROJECT_NAME "${PROJECT_NAME}-${CMAKE_GENERATOR_TOOLSET}")
endif()
if (CMAKE_GENERATOR_PLATFORM)
	set(PROJECT_NAME "${PROJECT_NAME}-${CMAKE_GENERATOR_PLATFORM}")
endif()

if(CMAKE_GENERATOR STREQUAL Xcode)
	project(${PROJECT_NAME} LANGUAGES C CXX OBJC OBJCXX VERSION ${PROJECT_VER})
else()
	project(${PROJECT_NAME} LANGUAGES C CXX)
endif()

project(${PROJECT_NAME} VERSION 0.1.0)

# IMPORTANT: must setup "project" before use "VCPKG_TARGET_TRIPLET"
axe_dump_var(VCPKG_TARGET_TRIPLET)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/deploy/_bin/${VCPKG_TARGET_TRIPLET}")
axe_dump_var(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

if (CMAKE_CONFIGURATION_TYPES)
    set(AXE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
else()
    set(AXE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

message("==== ${PROJECT_NAME} ====")
message("AXE_PROJECT_ROOT               = ${AXE_PROJECT_ROOT}")
#axe_dump_cmake_variables()
message("CMAKE_CURRENT_SOURCE_DIR       = ${CMAKE_CURRENT_SOURCE_DIR}")
message("CMAKE_CURRENT_BINARY_DIR       = ${CMAKE_CURRENT_BINARY_DIR}")
message("CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

find_package(fmt                CONFIG REQUIRED)
find_package(EABase             CONFIG REQUIRED)
find_package(EASTL              CONFIG REQUIRED)
find_package(nlohmann_json      CONFIG REQUIRED)
find_package(Tracy              CONFIG REQUIRED)

#-- externals ---
add_subdirectory(externals)

add_subdirectory(src/core)
add_subdirectory(src/render)
add_subdirectory(src/engine)
add_subdirectory(src/editor)
add_subdirectory(src/tools/editor_app)

#-- test ---
add_subdirectory(src/test/core)
add_subdirectory(src/test/render)
add_subdirectory(src/test/engine)
add_subdirectory(src/test/editor)

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT axe_editor_app) # Set As Startup Project