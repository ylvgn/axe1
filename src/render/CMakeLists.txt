set(PROJECT_NAME axe_render)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

axe_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER axe/render)
target_link_libraries(${PROJECT_NAME} PUBLIC axe_core)
target_link_libraries(${PROJECT_NAME} PRIVATE DirectX-Headers)

#--------------------
find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)

find_package(JPEG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC JPEG::JPEG)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)