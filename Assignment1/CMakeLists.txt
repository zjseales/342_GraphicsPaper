CMAKE_MINIMUM_REQUIRED(VERSION "3.12")

set(CMAKE_CXX_STANDARD 14)

project(Features)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(FlanBased FlannBasedTest.cpp Timer.h Timer.cpp)
target_link_libraries(FlanBased ${OpenCV_LIBS}
)