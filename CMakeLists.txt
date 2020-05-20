cmake_minimum_required(VERSION 3.5)

project(BatchProcessing LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
## indicate we wish for the binaries to be written to the bin directory
#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin/${CMAKE_BUILD_TYPE})
# we have custom CMake configuration files, tell CMake to use them:
LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

# Lib TIFF
# Tiff lib
FIND_LIBRARY(TIFF_LIBRARIES tiff
    HINTS
    ${CMAKE_SOURCE_DIR}/vendor/tifflib/lib
    REQUIRED)

SET(TIFF_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/vendor/tifflib/include )
SET(TIFF_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/vendor/tifflib/lib)
INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
LINK_DIRECTORIES(${TIFF_LIBRARY_DIR})

find_package(Qt5 COMPONENTS Widgets Core Quick REQUIRED)
# Cuda GPU
FIND_PACKAGE(CUDA REQUIRED)
FIND_PACKAGE(NVVM REQUIRED)

# Array Fire library
FIND_PACKAGE(ArrayFire REQUIRED)
INCLUDE_DIRECTORIES(${ArrayFire_INCLUDE_DIRS})
#WIN32
add_executable(BatchProcessing
    Tiff/TiffReader.cpp
    Tiff/TiffReader.h
    Tiff/TiffWriter.cpp
    Tiff/TiffWriter.h
    Tiff/Common.h
    Tiff/BioImage.cpp
    Tiff/BioImage.h
    JobProducer.cpp
    JobProducer.h
    Job.cpp
    Job.h
    JobQueue.cpp
    JobQueue.h
    DeskewRunner.cpp
    DeskewRunner.h
    HyperStackRunner.cpp
    HyperStackRunner.h
    Logger.cpp
    Logger.h
    main.cpp
    qml.qrc
    )


target_compile_definitions(${PROJECT_NAME}
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt5::Core Qt5::Quick Qt5::Widgets ${TIFF_LIBRARIES} ${ArrayFire_CUDA_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT} ${CUDA_LIBRARIES} ${NVVM_LIB})


 if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
     SET(WINDEPLOY_TYPE "debug")
 else()
     SET(WINDEPLOY_TYPE "release")
 endif()
message(${CMAKE_CURRENT_BINARY_DIR})

 message(STATUS ${WINDEPLOY_TYPE})
 get_filename_component(QT_BASE_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)

 add_custom_command(TARGET ${PROJECT_NAME}
     POST_BUILD
     COMMAND "C:/Qt/5.14.1/msvc2017_64/bin/windeployqt.exe" --${WINDEPLOY_TYPE} --compiler-runtime  --json --pdb --qmldir ${QT_BASE_DIR}/../qml "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe"
 )

 add_custom_command(TARGET ${PROJECT_NAME}
     POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy  ${ArrayFire_INCLUDE_DIRS}/../lib/afcuda.dll "${CMAKE_CURRENT_BINARY_DIR}/"
 )

 add_custom_command(TARGET ${PROJECT_NAME}
     POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy  ${TIFF_LIBRARY_DIR}/tiff.dll "${CMAKE_CURRENT_BINARY_DIR}/"
 )

