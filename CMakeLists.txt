cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(xpdToUsd VERSION 1.0)
include(cmake/utils.cmake)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/modules
)
find_package(Maya 2018 REQUIRED)
find_package(USD 0.20.05 REQUIRED)


add_executable(xpdToUsd "src/xpdToUsd.cpp")
target_include_directories(${PROJECT_NAME} 
    PUBLIC
        ${MAYA_INCLUDE_DIRS}
        ${MAYA_BASE_DIR}/plug-ins/xgen/include
        ${PXR_INCLUDE_DIRS}
)
target_link_libraries(xpdToUsd
PUBLIC
ar
gf
hd
hdx
js
kind
plug
sdf
tf
trace
usd
usdGeom
usdImaging
usdImagingGL
usdLux
usdRender
usdRi
usdShade
usdSkel
usdUtils
vt
${MAYA_LIBRARIES}
${MAYA_XGEN_LIBRARY}
)

#adding default build type
if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_custom_command(TARGET xpdToUsd
PRE_LINK
COMMENT "copying xpd dll"
COMMAND ${CMAKE_COMMAND}
VERBATIM
ARGS -E copy ${MAYA_BASE_DIR}/plug-ins/xgen/bin/AdskXpd.dll $<TARGET_FILE_DIR:xpdToUsd>/AdskXpd.dll)

foreach(LIB ${USD_SHARED_LIBS})
    get_filename_component(LIB_NAME ${LIB} NAME)
    add_custom_command(TARGET xpdToUsd
    PRE_LINK
    COMMENT "copying ${LIB_NAME} dll"
    VERBATIM
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${LIB} $<TARGET_FILE_DIR:xpdToUsd>/${LIB_NAME})
endforeach()