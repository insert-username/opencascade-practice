cmake_minimum_required(VERSION 3.5)

project(qt-example LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
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

find_package(OpenGL REQUIRED)

find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
FIND_PACKAGE(OpenCasCade REQUIRED)
message("OCE_FOUND? <${OCE_FOUND}>")
message("OCC_FOUND? <${OCC_FOUND}>")
message("OCC INCLUDE DIR: <${OCC_INCLUDE_DIR}>")
message("OCC Libraries: <${OCC_LIBRARIES}>")

INCLUDE_DIRECTORIES(${OCC_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})


INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5OpenGL_INCLUDE_DIRS})

message("OPENGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR}")
message("Qt5Core_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS}")
message("Qt5Gui_INCLUDE_DIRS ${Qt5Gui_INCLUDE_DIRS}")
message("Qt5Widgets_INCLUDE_DIRS ${Qt5Widgets_INCLUDE_DIRS}")
message("Qt5OpenGL_INCLUDE_DIRS ${Qt5OpenGL_INCLUDE_DIRS}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
