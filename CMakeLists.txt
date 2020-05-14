cmake_minimum_required(VERSION 3.16)

project(iar_project CXX)
set(iar_project_VERSION_MAJOR 0)
set(iar_project_VERSION_MINOR 6)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    message("Using LLD")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
    set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
endif()


add_subdirectory(lib/cnl         EXCLUDE_FROM_ALL)
add_subdirectory(lib/fmt         EXCLUDE_FROM_ALL)

add_library(toml++ INTERFACE IMPORTED)
target_include_directories(toml++ INTERFACE lib/tomlplusplus/include)

add_library(pcg INTERFACE IMPORTED)
target_include_directories(pcg INTERFACE lib/pcg-cpp/include)

add_executable(evo_boids
               src/Controller/main.cpp
               src/Controller/EvOverseer.cpp
               src/Controller/Sim.cpp
               src/Model/Agents/Agent.cpp
               src/Model/Agents/AgentVisitor.cpp
               src/Model/Agents/Predator.cpp
               src/Model/Agents/Prey.cpp
               src/Model/Agents/Retina.cpp
               src/Model/Agents/layer.cpp
               src/Model/Environment/Environment.cpp
               src/Model/Environment/torus.cpp
               src/Model/Environment/box.cpp
               src/Model/Evo/MarkovBrain.cpp
               src/Model/Evo/ProbaLogicGate.cpp
               src/View/MainView.cpp
               src/Misc/Utils.cpp
)

set_target_properties(evo_boids PROPERTIES CXX_STANDARD 20
                                           CXX_STANDARD_REQUIRED ON
                                           CXX_EXTENSIONS OFF
                                           INTERPROCEDURAL_OPTIMIZATION True)

target_include_directories(evo_boids PRIVATE src)

target_link_libraries(evo_boids PRIVATE Threads::Threads
                                        SDL2
                                        Cnl
                                        pcg
                                        fmt-header-only
                                        toml++)
