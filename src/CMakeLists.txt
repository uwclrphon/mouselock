project(mouse_lock_main LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON) # Enable AUTOMOC for Qt files
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(core)
add_subdirectory(json_utils)

File(GLOB srcs "*.cpp")

# Specify MSVC UTF-8 encoding   
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(${PROJECT_NAME}
    WIN32 # If you need a terminal for debug, please comment this statement 
    ${srcs} 
)

# Add dependency to ensure core is built first
add_dependencies(${PROJECT_NAME} core)

# Link Qt libraries manually
if(QT_ROOT)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        global_count
        core
        json_utils
    )
    
    # Add Qt include directories
    target_include_directories(${PROJECT_NAME} PRIVATE 
        "${QT_ROOT}/include"
        "${QT_ROOT}/include/QtCore"
        "${QT_ROOT}/include/QtGui"
        "${QT_ROOT}/include/QtWidgets"
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        global_count
        core
        json_utils
    )
endif()

# Add all necessary include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/json_utils
    ${PROJECT_SOURCE_DIR}/src/core
    ${PROJECT_SOURCE_DIR}/src/core/global_count
    ${PROJECT_SOURCE_DIR}/src/core/blue_screen
    ${PROJECT_SOURCE_DIR}/src/core/tools
    ${PROJECT_SOURCE_DIR}/src/utils
)

if(QT_ROOT)
    # Add Qt library directory
    link_directories("${QT_ROOT}/lib")
    # Add Qt include directories
    target_include_directories(${PROJECT_NAME} PRIVATE 
        "${QT_ROOT}/include"
        "${QT_ROOT}/include/QtCore"
        "${QT_ROOT}/include/QtGui"
        "${QT_ROOT}/include/QtWidgets"
    )
else()
    message(STATUS "Using system Qt paths")
endif()

# Link system libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        gdi32
        gdiplus
        user32
    )
endif()

# Install project libraries

# 复制配置文件到构建目录
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/global_config.json
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/
)
