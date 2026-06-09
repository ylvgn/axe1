# ref: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator

set(PROJECT_NAME VulkanMemoryAllocator)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

option(MY_VMA_HEADER_ONLY "default OFF" OFF)

if (MY_VMA_HEADER_ONLY)
	add_library(${PROJECT_NAME} INTERFACE)
else()
	# Usage: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/blob/master/include/vk_mem_alloc.h#L17258
	add_library(${PROJECT_NAME} STATIC)
endif()

file(GLOB_RECURSE all_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

if (MY_VMA_HEADER_ONLY)
	target_include_directories(${PROJECT_NAME} INTERFACE src)
else()
	target_include_directories(${PROJECT_NAME} PUBLIC src)
endif()

target_sources(${PROJECT_NAME} PRIVATE ${all_files})

if(CMAKE_GENERATOR MATCHES "Visual Studio")
	set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "externals")
endif()

if(WIN32)
	set(vulkan_sdk_path $ENV{VK_SDK_PATH})
	set(VkIncludeDir 	"${vulkan_sdk_path}/Include")
	set(VkLibDir 		"${vulkan_sdk_path}/Lib")
	
	if (MY_VMA_HEADER_ONLY)
		target_include_directories(${PROJECT_NAME} INTERFACE "${VkIncludeDir}")
		target_link_directories(   ${PROJECT_NAME} INTERFACE "${VkLibDir}")
	else()
		target_include_directories(${PROJECT_NAME} PUBLIC "${VkIncludeDir}")
		target_link_directories(   ${PROJECT_NAME} PUBLIC "${VkLibDir}")
		target_link_libraries(${PROJECT_NAME} PUBLIC vulkan-1)
	endif()
endif()

if (NOT MY_VMA_HEADER_ONLY)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(${PROJECT_NAME} PRIVATE
			-Wno-error
			-Wno-unused-private-field
			-Wno-unused-variable
			-Wno-nullability-completeness)
	endif()
endif()

