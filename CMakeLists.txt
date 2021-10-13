cmake_minimum_required(VERSION 3.19)
project(eglue_server
        VERSION 0.0.1
        LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)

add_executable(eglue_server main.cpp my_server.cpp my_server.h)

target_include_directories(eglue_server PRIVATE "lib/eglue-lib")
add_library(eglue-lib SHARED IMPORTED)
set_target_properties(eglue-lib PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/eglue-lib/cmake-build-debug/libeglue_lib.so)

target_link_libraries(eglue_server LINK_PUBLIC eglue-lib)

find_package(Threads COMPONENTS program_options REQUIRED)
target_link_libraries(eglue_server LINK_PUBLIC Threads::Threads)

find_package(Boost 1.76 COMPONENTS program_options REQUIRED)
target_compile_definitions(eglue_server PUBLIC -DBOOSTFOUND=${Boost_FOUND})

if (${Boost_FOUND})
    include_directories(${Boost_INCLUDE_DIR})
    target_link_libraries(eglue_server LINK_PUBLIC ${Boost_LIBRARIES})
endif ()

