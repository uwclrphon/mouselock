project(core)
# Add subdirectories in correct order
add_subdirectory(global_count)
add_subdirectory(blue_screen)
add_subdirectory(tools)

# Main executable
add_library(${PROJECT_NAME} SHARED
  core.cpp
)

# Enable import library generation on Windows
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()

# Add dependency to ensure tools is built first
add_dependencies(${PROJECT_NAME} tools)

# Link common configuration
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  common_config
)

# Link project libraries
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  global_count
  tools
  blue_screen
)

# Windows specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        kernel32.lib
        user32.lib
    )
endif()


# Add explicit install target
add_custom_target(install_all
    COMMAND ${CMAKE_COMMAND} --install .
    COMMENT "Installing all components"
    DEPENDS ${PROJECT_NAME}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/blue.png
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blue.png
  COMMENT "Copying blue.png to output directory"
)