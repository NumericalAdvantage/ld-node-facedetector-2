# This file is part of project link.developers/ld-node-facedetector-2.
# It is copyrighted by the contributors recorded in the version control history of the file,
# available from its original location https://gitlab.com/link.developers.beta/ld-node-facedetector-2.
#
# SPDX-License-Identifier: MPL-2.0

cmake_minimum_required(VERSION 3.0)
project(ld-node-facedetector-2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Link2-Cpp REQUIRED)
find_package(Flatc REQUIRED)
find_package(ld-cmake-tools REQUIRED)
find_package(ld-lib-image REQUIRED)
find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)
find_package(protobuf CONFIG REQUIRED)

# When adding new data formats, add them to the INPUT_FILES section.
# If you use libraries that contain Flatbuffer files, uncomment the lines with LD_FLATBUFFER_ variables.
flatc_generate_cpp(
    BFBS_OUTPUT_FOLDER ${CMAKE_SOURCE_DIR}/data
    INCLUDE_PATHS ${LD_FLATBUFFER_DATA_FOLDERS}
    INPUT_FILES
        ${LD_FLATBUFFER_FILES}
        data/ImageWithBoundingBoxes.fbs
    )

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Facedetector.cpp
    src/Facedetector.hpp
    ${FLATC_GENERATED_SOURCES}
    )

target_include_directories(${PROJECT_NAME} PRIVATE ${FLATC_GENERATED_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} dlib::dlib)
target_link_libraries(${PROJECT_NAME} PUBLIC DRAIVE::Link2-Cpp ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS} dlib::dlib protobuf::libprotobuf link_dev::ld-lib-image)

cmake_make_installation(
        TARGETS ${PROJECT_NAME}
        LINK2_STATIC_ASSETS
            specification.json
            data
        IS_EXECUTABLE)
