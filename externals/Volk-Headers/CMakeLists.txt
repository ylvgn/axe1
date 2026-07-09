# ref: https://github.com/zeux/volk

set(PROJECT_NAME Volk-Headers)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

file(GLOB_RECURSE all_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

#if(WIN32)
#	set(vulkan_sdk_path $ENV{VK_SDK_PATH})
#	set(VkIncludeDir 	"${vulkan_sdk_path}/Include")
#	set(VkLibDir 		"${vulkan_sdk_path}/Lib")
#endif()

add_library(${PROJECT_NAME} INTERFACE)
target_compile_definitions(${PROJECT_NAME} INTERFACE VK_NO_PROTOTYPES)
#target_include_directories(${PROJECT_NAME} INTERFACE "${VkIncludeDir}")
target_include_directories(${PROJECT_NAME} INTERFACE src)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
	set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "externals")
endif()


