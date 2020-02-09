cmake_minimum_required(VERSION 3.16)

project(iar_project CXX)
set(iar_project_VERSION_MAJOR 0)
set(iar_project_VERSION_MINOR 6)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)

add_subdirectory(lib/cnl EXCLUDE_FROM_ALL)

add_executable(evo_boids
               src/Controller/main.cpp
               src/Controller/LightSim.cpp
               src/Controller/LocalThreadSim.cpp
               src/Model/Agents/Agent.cpp
               src/Model/Agents/Predator.cpp
               src/Model/Agents/Prey.cpp
               src/Model/Agents/Retina.cpp
               src/Model/Environment/Environment.cpp
               src/Model/Environment/torus.cpp
               src/Model/Environment/box.cpp
               src/Model/Evo/MarkovBrain.cpp
               src/Model/Evo/ProbaLogicGate.cpp
               src/View/MainView.cpp
               src/Misc/Utils.cpp
              )

set_target_properties(evo_boids PROPERTIES
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON
                      CXX_EXTENSIONS OFF)

target_include_directories(evo_boids PRIVATE src)

target_link_libraries(evo_boids PRIVATE Threads::Threads SDL2 Cnl)
