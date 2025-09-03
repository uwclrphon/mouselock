project(json_utils LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Use Qt paths from parent
if(QT_ROOT)
    # Qt configuration
    link_directories("${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/lib")

    add_library(${PROJECT_NAME} SHARED
        json.cpp
    )

    # Print debug info
    message(STATUS "Using Qt from: ${QT_ROOT}/${QT_VERSION}/${QT_ARCH}")
    message(STATUS "Qt library paths:")
    message(STATUS "  Core: ${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/bin/Qt6Core.dll")
    message(STATUS "  Gui: ${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/bin/Qt6Gui.dll")
    message(STATUS "  Widgets: ${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/bin/Qt6Widgets.dll")
    
    # Link Qt libraries manually
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        "${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/bin/Qt6Core.dll"
        "${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/bin/Qt6Gui.dll"
        "${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/bin/Qt6Widgets.dll"
        common_config
    )

    # Include directories
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/json_utils
        ${CMAKE_SOURCE_DIR}/src
        "${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/include"
        "${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/include/QtCore"
    )
else()
    # Qt configuration
    link_directories(${QT_LIBRARY_DIR})

    add_library(${PROJECT_NAME} SHARED
        json.cpp
    )

    # Link Qt libraries manually
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        "${QT_LIBRARY_DIR}/../bin/Qt6Core.dll"
        "${QT_LIBRARY_DIR}/../bin/Qt6Gui.dll"
        "${QT_LIBRARY_DIR}/../bin/Qt6Widgets.dll"
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
