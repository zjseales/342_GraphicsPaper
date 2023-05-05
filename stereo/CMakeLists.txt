CMAKE_MINIMUM_REQUIRED(VERSION "3.12")

set(CMAKE_CXX_STANDARD 14)

project(Stereo)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(stereo stereo.cpp CalibrationIO.h CalibrationIO.cpp)
target_link_libraries(stereo ${OpenCV_LIBS}
)