project(blue_screen
  VERSION 0.1.0
  LANGUAGES CXX
)

add_library(${PROJECT_NAME} SHARED
  blue_screen.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}
  ${GLOBAL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/include
)

# 链接依赖库
target_link_libraries(${PROJECT_NAME} PRIVATE
  gdi32
  gdiplus.lib
  common_config
  global_count
)