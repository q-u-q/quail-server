cmake_minimum_required(VERSION 3.10)
project(quail-server-demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# set(WEBRTC_ENABLE_LOG ON)
# set(DISABLE_LOG4CXX ON)

add_subdirectory(".." build)

add_executable(${PROJECT_NAME}
        main.cc)

target_link_libraries(${PROJECT_NAME}
        quail-server)
