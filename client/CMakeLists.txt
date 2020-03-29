cmake_minimum_required(VERSION 3.5)

project(ftpClient LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# TODO 那些函数是 CMAKE 的内置函数，这个效果为什么 CMAKE_CXX_FLAGS 中间的参数 ?
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -g -m64")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -lssl -lcrypto -m64 -lpthread")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# TODO 如何修改为相对路径 ?
include_directories("/home/shen/Core/homework/eomaia/eomaia")
if(ANDROID)
  add_library(ftpClient SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
  )
else()
  add_executable(ftpClient
    main.cpp
    LoginDialog.cpp
    mainwindow.cpp
    TcpClient.cpp
    mainwindow.h
    mainwindow.ui
  )
endif()

target_link_libraries(ftpClient PRIVATE Qt5::Widgets /home/shen/Core/homework/eomaia/libeomaia_static.a)
