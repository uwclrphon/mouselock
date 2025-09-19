project(json_utils LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Use Qt paths from parent
if(QT_ROOT)
    add_library(${PROJECT_NAME} SHARED
        json.cpp
    )
    
    # Enable import library generation on Windows
    if(WIN32)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
        )
    endif()

    # Link Qt libraries using imported targets
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        Qt6::Core
        Qt6::Gui
        common_config
    )

    # Include directories
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/json_utils
        ${CMAKE_SOURCE_DIR}/src
    )
else()
    # Qt configuration
    link_directories(${QT_LIBRARY_DIR})

    add_library(${PROJECT_NAME} SHARED
        json.cpp
    )

    # Link Qt libraries manually
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        Qt6::Core
        Qt6::Gui
        common_config
    )

    # Include directories
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/json_utils
        ${CMAKE_SOURCE_DIR}/src
    )
endif()

# Link system libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        gdi32
        user32
    )
endif()
