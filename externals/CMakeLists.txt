include(../axe_common.cmake)

set(PROJECT_NAME _axe_externals)
message("==== ${PROJECT_NAME} ====")
# project(${PROJECT_NAME}) # !!<--- no need this, cuz Not A real project only for command cmake script

# https://cmake.org/cmake/help/latest/command/add_custom_target.html#command:add_custom_target
add_custom_target(${PROJECT_NAME} ALL)
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER axe)

#-- vcpkg tools ---
if(WIN32)
    set(axe_glslc "${CMAKE_CURRENT_SOURCE_DIR}/_vcpkg/installed/x64-windows/tools/shaderc/glslc.exe")
    set(axe_spirv_cross "${CMAKE_CURRENT_SOURCE_DIR}/_vcpkg/installed/x64-windows/tools/spirv-cross/spirv-cross.exe")
    set(axe_ninja "${CMAKE_CURRENT_SOURCE_DIR}/_vcpkg/installed/x64-windows/tools/ninja/ninja.exe")
else()
    axe_log_error("unsupported platform")
endif()

axe_copy_files_post_build(${PROJECT_NAME}
    ${axe_glslc}
    ${axe_spirv_cross}
    ${axe_ninja}
#----
    ${AXE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION}
)

#-- vcpkg python3 ---
axe_vcpkg_find_python(axe_python3 3)
axe_copy_dir_post_build(${PROJECT_NAME} ${axe_python3} ${AXE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION}/python3)
