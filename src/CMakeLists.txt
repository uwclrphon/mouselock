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
# Link Qt libraries manually
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6Core.lib
    Qt6Gui.lib
    Qt6Widgets.lib
    global_count
    core
    json_utils
)

# Add Qt library directory
link_directories("D:/Qt/6.9.0/mingw_64/lib")

# Add all necessary include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/json_utils
    ${PROJECT_SOURCE_DIR}/src/core
    ${PROJECT_SOURCE_DIR}/src/core/global_count
    ${PROJECT_SOURCE_DIR}/src/core/blue_screen
    ${PROJECT_SOURCE_DIR}/src/core/tools
    ${PROJECT_SOURCE_DIR}/src/utils
    "D:/Qt/6.9.0/mingw_64/include"
    "D:/Qt/6.9.0/mingw_64/include/QtCore"
    "D:/Qt/6.9.0/mingw_64/include/QtGui"
    "D:/Qt/6.9.0/mingw_64/include/QtWidgets"
)

# Link system libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        gdi32
        gdiplus
        user32
    )
endif()

# Install project libraries
