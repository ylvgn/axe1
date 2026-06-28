# ref: https://github.com/zeux/volk

set(PROJECT_NAME Volk)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC)
target_compile_definitions(${PROJECT_NAME} PRIVATE VK_NO_PROTOTYPES)

file(GLOB_RECURSE all_files "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*")

target_include_directories(${PROJECT_NAME} PUBLIC src)
#target_sources(${PROJECT_NAME} PUBLIC ${all_files})
target_sources(${PROJECT_NAME} PRIVATE src/Volk.cpp) # !!<-- Do not compile volk.c


if(CMAKE_GENERATOR MATCHES "Visual Studio")
	set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER "externals")
endif()

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE $ENV{VULKAN_SDK}/include)

#if(WIN32)
#	set(vulkan_sdk_path $ENV{VK_SDK_PATH})
#	set(VkIncludeDir 	"${vulkan_sdk_path}/Include")
#	set(VkLibDir 		"${vulkan_sdk_path}/Lib")
#	
#	target_include_directories(${PROJECT_NAME} PUBLIC "${VkIncludeDir}")
#	target_link_directories(   ${PROJECT_NAME} PUBLIC "${VkLibDir}")
#endif()

