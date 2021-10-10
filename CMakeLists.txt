cmake_minimum_required(VERSION 3.19)
project(eglue_server
        VERSION 0.0.1
        LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)

find_package( Boost 1.76 COMPONENTS program_options REQUIRED )
add_executable(eglue_server main.cpp)

target_compile_definitions(eglue_server PUBLIC -DBOOSTFOUND=${Boost_FOUND})
target_include_directories(eglue_server PRIVATE "lib/eglue-lib")
add_library(eglue-lib SHARED IMPORTED)
set_target_properties( eglue-lib PROPERTIES IMPORTED_LOCATION /home/matteo/Documents/eGlue/eglue-server/lib/eglue-lib/cmake-build-debug/libeglue_lib.so )

target_link_libraries(eglue_server LINK_PUBLIC eglue-lib)

#set_target_properties( eglue-lib PROPERTIES IMPORTED_LOCATION "lib/eglue-lib/cmake-build-debug/libeglue_lib.so" )
#target_link_libraries(eglue_server LINK_PUBLIC eglue-lib)