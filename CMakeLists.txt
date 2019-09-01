cmake_minimum_required(VERSION 3.10)
project(flow_reader)
find_package( OpenCV REQUIRED )
set(CMAKE_CXX_STANDARD 11)

find_library(MLPACK_LIBRARY
        NAMES mlpack
        PATHS "/usr/lib64/ /usr/lib/ /usr/local/lib64/ /usr/local/"
        )

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

include_directories( ${OpenCV_INCLUDE_DIRS})


add_executable(flow_reader main.cpp flow.h flow.cpp Dbscan.cpp Dbscan.h Xycut.cpp Xycut.h common.cpp common.h)
target_link_libraries(flow_reader ${OpenCV_LIBS})