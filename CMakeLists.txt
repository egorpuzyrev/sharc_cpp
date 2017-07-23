cmake_minimum_required(VERSION 3.7)

project(sharc_cpp)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()


IF(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
  message(AUTHOR_WARNING "${Red}CMAKE_BUILD_TYPE Debug by default${ColourReset}")
ENDIF()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost COMPONENTS iostreams REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

add_compile_options(
    -Wall
    -std=c++1y
    -fexceptions
    -static
    -static-libstdc++
    -fPIC
    -march=core2
    -lboost_iostreams
    -fopenmp
    -lz
    -lbz2
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -lgcov -O0 -g -pg")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2 -s")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#add_library(ahocorasik STATIC ahocorasik.cpp)
#add_library(base64 STATIC base64.cpp)
#add_library(compressor STATIC compressor.cpp)
#add_library(keys STATIC keys.cpp)
#add_library(lcp STATIC lcp.cpp)
#add_library(rabinkarp STATIC rabinkarp.cpp)
#add_library(statistics STATIC statistics.cpp)
#add_library(support STATIC support.cpp)
#
#add_executable(${PROJECT_NAME} main.cpp)
#
#target_link_libraries(${PROJECT_NAME} compressor boost_iostreams keys ahocorasik base64 lcp rabinkarp statistics support)

add_executable(${PROJECT_NAME}
    main.cpp
    compressor.cpp
    keys.cpp
    ahocorasik.cpp
    base64.cpp
    lcp.cpp
    rabinkarp.cpp
    statistics.cpp
    support.cpp
)

target_link_libraries(${PROJECT_NAME} boost_iostreams z bz2)
