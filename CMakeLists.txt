cmake_minimum_required(VERSION 3.16)

project(iar_project CXX)
set(iar_project_VERSION_MAJOR 0)
set(iar_project_VERSION_MINOR 6)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        11.2.0
)

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-2.32.6
)

FetchContent_Declare(
  Tracy
  GIT_REPOSITORY https://github.com/wolfpld/tracy.git
  GIT_TAG        71fc3bc747f53bb8091f20fac186473d3e6a3003
)
FetchContent_MakeAvailable(SDL2 fmt Tracy)

add_library(toml++ INTERFACE IMPORTED)
target_include_directories(toml++ INTERFACE lib/tomlplusplus/include)

add_library(pcg INTERFACE IMPORTED)
target_include_directories(pcg INTERFACE lib/pcg-cpp/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

option(BUILD_TESTS "Build test programs" OFF)
if(BUILD_TESTS)

endif()

add_executable(evo_boids
               src/Controller/main.cpp
               src/Controller/EvOverseer.cpp
               src/Controller/Sim.cpp
               src/Model/Agents/Predator.cpp
               src/Model/Environment/Environment.cpp
               src/Model/Environment/torus.cpp
               src/Model/Environment/box.cpp
               src/Model/Evo/MarkovBrain.cpp
               src/Model/Evo/ProbaLogicGate.cpp
               src/View/MainView.cpp
)

set_target_properties(evo_boids PROPERTIES CXX_STANDARD 23
                                           CXX_STANDARD_REQUIRED ON
                                           CXX_EXTENSIONS OFF
                                           INTERPROCEDURAL_OPTIMIZATION True)

target_include_directories(evo_boids PRIVATE src)

target_link_libraries(evo_boids PRIVATE Threads::Threads
                                        SDL2
                                        pcg
                                        fmt
                                        toml++)

file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/rsrc
                 ${CMAKE_CURRENT_BINARY_DIR}/rsrc
                 RESULT COMPDB_LN_RES
                 SYMBOLIC)
file(CREATE_LINK ${CMAKE_CURRENT_BINARY_DIR}/
                 ${CMAKE_CURRENT_SOURCE_DIR}/current_build
                 RESULT COMPDB_LN_RES
                 SYMBOLIC)
file(CREATE_LINK ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
                 ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
                 RESULT COMPDB_LN_RES
                 SYMBOLIC)