include(../axe_common.cmake)

set(PROJECT_NAME _axe_externals)
message("==== ${PROJECT_NAME} ====")
# project(${PROJECT_NAME}) # !!<--- no need this, cuz Not A real project only for command cmake script

# https://cmake.org/cmake/help/latest/command/add_custom_target.html#command:add_custom_target
add_custom_target(${PROJECT_NAME} ALL)
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "externals")

#-- vcpkg tools ---
if(WIN32)
	#set(axe_glslc "${CMAKE_CURRENT_SOURCE_DIR}/_vcpkg/installed/x64-windows/tools/shaderc/glslc.exe")
	#set(axe_spirv_cross "${CMAKE_CURRENT_SOURCE_DIR}/_vcpkg/installed/x64-windows/tools/spirv-cross/spirv-cross.exe")
	#set(axe_ninja "${CMAKE_CURRENT_SOURCE_DIR}/_vcpkg/installed/x64-windows/tools/ninja/ninja.exe")
	set(axe_TracyClient "${CMAKE_CURRENT_SOURCE_DIR}/_vcpkg/installed/x64-windows/bin/TracyClient.dll") # tracy no static lib in vcpkg and cmake workflow
else()
    axe_log_error("unsupported platform")
endif()

axe_copy_files_post_build(${PROJECT_NAME}
	#${axe_glslc}
	#${axe_spirv_cross}
	#${axe_ninja}
	${axe_TracyClient}
#----
    ${AXE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION}
)

#-- vcpkg python3 ---
#axe_vcpkg_find_python(axe_python3 3)
#axe_copy_dir_post_build(${PROJECT_NAME} ${axe_python3} ${AXE_RUNTIME_OUTPUT_DIRECTORY_CONFIGURATION}/python3)

#add_subdirectory(assimp)
add_subdirectory(DirectX-Headers)
add_subdirectory(D3D12MemoryAllocator)
add_subdirectory(VulkanMemoryAllocator)
