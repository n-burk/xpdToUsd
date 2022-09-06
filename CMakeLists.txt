cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(xpdToUsd VERSION 1.0)
include(cmake/utils.cmake)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/modules
)
find_package(Maya 2023 REQUIRED)
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