add_executable(gol2p gol.cpp)

find_package(
  SDL2 CONFIG
  HINTS $ENV{HOME} /usr/local /opt/local /opt
)
if(SDL2_FOUND)
  if(SDL2_VERSION VERSION_LESS "2.0.12")
    target_include_directories(gol2p PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(gol2p PRIVATE ${SDL2_LIBRARIES})
  else()
    target_link_libraries(gol2p PRIVATE
      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
      $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    )
  endif()
else()
  include(FetchContent)
  message(STATUS "Fetching SDL2 library...")
  FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.30.5
  )
  FetchContent_MakeAvailable(SDL)
  target_link_libraries(gol2p PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  )
endif()
