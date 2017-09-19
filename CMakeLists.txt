cmake_minimum_required(VERSION 3.8)

project(iar_project CXX)
set(iar_project_VERSION_MAJOR 0)
set(iar_project_VERSION_MINOR 6)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)

set(controller_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Controller)
set(misc_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Misc)
set(model_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Model)
set(agents_SOURCE_DIR ${model_SOURCE_DIR}/Agents)
set(environment_SOURCE_DIR ${model_SOURCE_DIR}/Environment)
set(evo_SOURCE_DIR ${model_SOURCE_DIR}/Evo)
set(view_SOURCE_DIR ${PROJECT_SOURCE_DIR}/View)

include_directories(${controller_SOURCE_DIR}
                    ${agents_SOURCE_DIR}
                    ${environment_SOURCE_DIR}
                    ${evo_SOURCE_DIR}
                    ${view_SOURCE_DIR})

add_executable(${PROJECT_NAME}
               ${controller_SOURCE_DIR}/main.cpp
               ${controller_SOURCE_DIR}/LightSim.cpp
               ${controller_SOURCE_DIR}/LightSim.h
               ${controller_SOURCE_DIR}/LocalThreadSim.cpp
               ${controller_SOURCE_DIR}/LocalThreadSim.h
               ${misc_SOURCE_DIR}/Globals.h
               ${misc_SOURCE_DIR}/Utils.cpp
               ${misc_SOURCE_DIR}/Utils.h
               ${agents_SOURCE_DIR}/Agent.cpp
               ${agents_SOURCE_DIR}/Agent.h
               ${agents_SOURCE_DIR}/Predator.cpp
               ${agents_SOURCE_DIR}/Predator.h
               ${agents_SOURCE_DIR}/Prey.cpp
               ${agents_SOURCE_DIR}/Prey.h
               ${agents_SOURCE_DIR}/Retina.cpp
               ${agents_SOURCE_DIR}/Retina.h
               ${environment_SOURCE_DIR}/Environment.cpp
               ${environment_SOURCE_DIR}/Environment.h
               ${evo_SOURCE_DIR}/MarkovBrain_copy.cpp
               ${evo_SOURCE_DIR}/MarkovBrain_copy.h
               ${evo_SOURCE_DIR}/ProbaLogicGate.cpp
               ${evo_SOURCE_DIR}/ProbaLogicGate.h
               ${view_SOURCE_DIR}/MainView.cpp
               ${view_SOURCE_DIR}/MainView.h
               ${view_SOURCE_DIR}/SDLWrappers.h)

target_link_libraries(${PROJECT_NAME} Threads::Threads ${SDL2_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON
                      CXX_EXTENSIONS OFF)

