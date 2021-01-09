cmake_minimum_required(VERSION 3.5)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB SOURCES *.cpp *.h *.ui)

if(ANDROID)
  add_library(qt-example SHARED
    main.cpp
    ${SOURCES}
  )
else()
  add_executable(qt-example
    main.cpp
    ${SOURCES}
  )
endif()

set(OPENCC_LIBS
    TKOpenGl
    TKV3d
    TKFillet
    TKMesh
    TKernel
    TKG2d
    TKG3d
    TKMath
    TKIGES
    TKSTL
    TKShHealing
    TKXSBase
    TKBool
    TKBO
    TKBRep
    TKTopAlgo
    TKGeomAlgo
    TKGeomBase
    TKOffset
    TKPrim
    TKSTEPBase
    TKSTEPAttr
    TKSTEP209 TKSTEP
    TKHLR
    TKFeat)

target_link_libraries(qt-example GL OpenGL Qt5::Core Qt5::OpenGL Qt5::Gui Qt5::Widgets ${OPENCC_LIBS})
