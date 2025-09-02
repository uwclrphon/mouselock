project(json_utils)

add_library(${PROJECT_NAME} SHARED
    json.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    common_config
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)
