set(PROJECT_NAME axe_render)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

axe_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC axe_core)
target_link_libraries(${PROJECT_NAME} PRIVATE DirectX-Headers D3D12MemoryAllocator VulkanMemoryAllocator)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "axe")

#--------------------
find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)

find_package(JPEG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC JPEG::JPEG)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)