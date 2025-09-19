project(tools)
# Tools library
add_library(${PROJECT_NAME} SHARED
  mouse.cpp
  keyword.cpp
  times.cpp
)

# Enable import library generation on Windows
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()

# Add dependency to ensure blue_screen is built first
add_dependencies(${PROJECT_NAME} blue_screen)

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