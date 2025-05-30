set(PROJECT_NAME axe_core)
message("==== ${PROJECT_NAME} ====")
project(${PROJECT_NAME})

axe_add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER axe/core)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt EASTL nlohmann_json::nlohmann_json)