#
# Copyright (c) 2024 Armin Sobhani
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
cmrc_add_resource_library(
  gol2p-resources
  ALIAS gol2p::rc
  NAMESPACE gol2p
  font.ttf
  )
set(SDL2_LIBS gol2p::rc)

add_executable(${PROJECT_NAME} gol.cpp)

find_package(
  SDL2 CONFIG
  HINTS $ENV{HOME} /usr/local /opt/local /opt
)
if(NOT SDL2_FOUND)
  CHECK_LIBRARY_EXISTS(SDL2 SDL_Init "" SDL2_EXISTS)
  if(SDL2_EXISTS)
    list(APPEND SDL2_LIBS SDL2)
  else()
    message(STATUS "Fetching SDL2 library...")
    FetchContent_Declare(
      SDL
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG        release-2.30.5
    )
    FetchContent_MakeAvailable(SDL)
    list(APPEND SDL2_LIBS SDL2::SDL2-static)
  endif()
else()
  if(SDL2_VERSION VERSION_LESS "2.0.12")
    target_include_directories(gol2p PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(gol2p PRIVATE ${SDL2_LIBRARIES})
  else()
    list(APPEND SDL2_LIBS $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
  endif()
endif()

find_package(
  SDL2_ttf CONFIG
  HINTS $ENV{HOME} /usr/local /opt/local /opt
)
if(NOT SDL2_ttf_FOUND)
  CHECK_LIBRARY_EXISTS(SDL2_ttf TTF_Init "" SDL2_ttf_EXISTS)
  if(SDL2_ttf_EXISTS)
    list(APPEND SDL2_LIBS SDL2_ttf)
  else()
    message(STATUS "Fetching SDL2_ttf library...")
    FetchContent_Declare(
      SDL_ttf
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
      GIT_TAG        release-2.22.0
    )
    # set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build the library as a shared library")
    FetchContent_MakeAvailable(SDL_ttf)
    list(APPEND SDL2_LIBS SDL2_ttf::SDL2_ttf)
  endif()
else()
  list(APPEND SDL2_LIBS $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBS})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})