cmake_minimum_required(VERSION 3.10)
project(flow_reader)
find_package( OpenCV REQUIRED )
set(CMAKE_CXX_STANDARD 11)

find_library(MLPACK_LIBRARY
        NAMES mlpack
        PATHS "/usr/lib64/ /usr/lib/ /usr/local/lib64/ /usr/local/"
        )

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

find_package(Boost 1.69.0 REQUIRED)
find_package(flann REQUIRED)

include_directories( ${flann_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})


add_executable(flow_reader main.cpp xxhash.h xxhash.c lz4hc.h lz4hc.c lz4frame_static.h lz4frame.h lz4frame.c lz4.h lz4.c flow.h flow.cpp Dbscan.cpp Dbscan.h Enclosure.h Enclosure.cpp ImageNode.h ImageNode.cpp Xycut.cpp Xycut.h common.cpp common.h PageSegmenter.h PageSegmenter.cpp)
target_link_libraries(flow_reader ${OpenCV_LIBS} ${Boost_LIBRARIES})
