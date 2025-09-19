project(global_count)
# Global count library
add_library(${PROJECT_NAME} SHARED
  global_count.cpp
)

# Include headers
target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
)

# Link common configuration
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  common_config
  json_utils
)

# MSVC specific settings
if(MSVC)
  # Add import library directory
  target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}/src/json_utils/${CMAKE_BUILD_TYPE}
  )
endif()

# MSVC specific settings
if(MSVC)
  # Add DLL export definitions
  target_compile_definitions(${PROJECT_NAME} PRIVATE
      $<$<BOOL:${WIN32}>:BUILDING_GLOBAL_COUNT_DLL>
  )
endif()

# Add DLL export definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<BOOL:${WIN32}>:BUILDING_GLOBAL_COUNT_DLL>
)

# Link project libraries
# Removed blue_screen to break circular dependency