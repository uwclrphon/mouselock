project(mouse_lock_main)
# Main executable
add_executable(${PROJECT_NAME}
  main.cpp
)

# Link common configuration
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  common_config
)

# Add subdirectories
add_subdirectory(global_count)
add_subdirectory(tools)
add_subdirectory(blue_screen)

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

add_custom_command(TARGET mouse_lock_main POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/blue.png
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blue.png
  COMMENT "Copying blue.png to output directory"
)