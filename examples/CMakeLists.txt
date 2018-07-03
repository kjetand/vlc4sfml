project(vlc4sfml-example)

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

include_directories(${VLC4SFML_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} example.cpp)

target_link_libraries(${PROJECT_NAME} vlc4sfml)
