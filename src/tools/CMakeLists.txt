project(tools)
# Tools library
add_library(${PROJECT_NAME} SHARED
  mouse.cpp
  keyword.cpp
  times.cpp
)

# Link common configuration
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  common_config
)

# Link project libraries
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  global_count
  blue_screen
)