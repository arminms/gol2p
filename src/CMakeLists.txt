find_package(
  SDL2 CONFIG
  HINTS $ENV{HOME} /usr/local /opt/local /opt
)
if(NOT SDL2_FOUND)
  message(STATUS "Fetching SDL2 library...")
  FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        v2.30.5
  )
  FetchContent_MakeAvailable(SDL)
endif()

add_executable(gol gol.cpp)

if(SDL2_VERSION VERSION_LESS "2.0.12")
  target_include_directories(gol PRIVATE ${SDL2_INCLUDE_DIRS})
  target_link_libraries(gol PRIVATE ${SDL2_LIBRARIES})
else()
  target_link_libraries(gol PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  )
endif()

