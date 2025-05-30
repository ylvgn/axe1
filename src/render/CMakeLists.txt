set(PROJECT_NAME axe_render)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

axe_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER axe/render)

target_link_libraries(${PROJECT_NAME} PUBLIC axe_core imgui::imgui PNG::PNG)