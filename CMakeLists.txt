cmake_minimum_required(VERSION 3.20)
project(SQLCache)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

include_directories("/usr/include/cppconn")
include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(sqlcache ${SOURCES})

target_link_libraries(sqlcache -lmysqlcppconn)
