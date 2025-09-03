cmake_minimum_required(VERSION 3.15)

# Project configuration
project(mouse_lock
  VERSION 0.1.0
  DESCRIPTION "Mouse and keyboard input monitoring tool"
  LANGUAGES CXX
)

# Qt configuration
set(QT_ROOT "" CACHE PATH "Path to Qt installation root")
set(QT_VERSION "6.9.0" CACHE STRING "Qt version")
set(QT_ARCH "mingw_64" CACHE STRING "Qt architecture")

if(QT_ROOT)
    set(CMAKE_PREFIX_PATH "${QT_ROOT}/${QT_VERSION}/${QT_ARCH}")
    set(QT_DIR "${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/lib/cmake/Qt6")
    find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

    # Set Qt library paths
    set(QT_LIBRARY_DIR "${QT_ROOT}/${QT_VERSION}/${QT_ARCH}/lib")
    link_directories(${QT_LIBRARY_DIR})
else()
    message(WARNING "Qt root path not specified, trying system paths")
    find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
endif()

# Windows specific settings
if(WIN32)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
endif()

# Standard and properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTING "Build tests" ON)
option(if_debug "Enable debug output if true" OFF)

# Set debug flag based on if_debug option
if(if_debug)
    add_compile_definitions(ENABLE_DEBUG_MODE=1)
    message(STATUS "Debug output enabled")
else()
    message(STATUS "Debug output disabled - Release mode")
endif()

# Directory structure
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(GLOBAL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(JSON_UTILS ${PROJECT_SOURCE_DIR}/src/json_utils)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Check for optional OpenCV
find_package(OpenCV)

if(OpenCV_FOUND)
    message(STATUS "OpenCV found (optional)")
    message(STATUS "OpenCV_DIR = ${OpenCV_DIR}")
    message(STATUS "OpenCV libraries = ${OpenCV_LIBS}")
else()
    message(WARNING "You don't have opencv, but it doesn't affect project compilation, and future projects may use opencv")
endif()

# Common target properties
add_library(common_config INTERFACE)
target_include_directories(common_config INTERFACE 
  ${GLOBAL_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/global_count
)
target_compile_options(common_config INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror>
)

# Add subdirectories
add_subdirectory(${SRC_DIR})

# Standard installation paths
include(GNUInstallDirs)

# Install executable
install(TARGETS mouse_lock_main
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Packaging
include(CPack)