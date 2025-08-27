project(camera
  VERSION 0.1.0
  LANGUAGES CXX
)

find_package(OpenCV REQUIRED)

add_library(${PROJECT_NAME} SHARED
  camera_utils.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}
  ${OpenCV_INCLUDE_DIRS}
  ${GLOBAL_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  ${OpenCV_LIBS}
  common_config
  global_count
)