cmake_minimum_required(VERSION 3.9)
project(XimalayaTextDownload)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14")

add_executable(main
    Lib/Exception.cpp
    Lib/Curl.cpp
    Lib/StringOperation.cpp
    Lib/lodepng.cpp
    Picture.cpp
    Captcha.cpp
    Ximalaya.cpp
    XimalayaAlbum.cpp
    XimalayaTrack.cpp
    RTFWriter.cpp
    main.cpp)
target_include_directories(main PUBLIC Lib)

find_library(LIBCURL curl REQUIRED)
find_library(LIBJPEG jpeg REQUIRED)
set(LIBS
    ${LIBCURL}
    ${LIBJPEG}
)
target_link_libraries(main PRIVATE ${LIBS})
