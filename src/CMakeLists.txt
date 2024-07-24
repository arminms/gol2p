find_package(SDL2 REQUIRED)

add_executable(gol gol.cpp)
target_include_directories(gol PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(gol PRIVATE ${SDL2_LIBRARIES})