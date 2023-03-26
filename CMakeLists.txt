cmake_minimum_required(VERSION 3.21)

project(HttpServer
        VERSION 0.0.0
        LANGUAGES C)

set(CMAKE_C_STANDARD 17)

add_subdirectory(core-lib)
add_subdirectory(poll-server)
add_subdirectory(core)

target_link_libraries(poll-server PUBLIC core-lib)
target_link_libraries(core PUBLIC core-lib)
add_dependencies(core poll-server)
add_dependencies(poll-server core-lib)