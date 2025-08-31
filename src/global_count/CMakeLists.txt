project(global_count)
# Global count library
add_library(${PROJECT_NAME} SHARED
  global_count.cpp
)

# Include headers
target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link common configuration
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  common_config
)

# Link project libraries
# Removed blue_screen to break circular dependency