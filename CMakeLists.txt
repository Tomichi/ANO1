cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Ofast")

# Define project name
project(ANO1)

# Fincom.apple.dt.XCoded OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)

# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

include_directories(${OpenCV_INCLUDE_DIRS})


# Declare the executable target built from your sources
add_executable(ANO1 ImageObject.cpp ImageObject.h Etalons.cpp Etalons.h main.cpp)

# Link your application with OpenCV libraries
target_link_libraries(ANO1 ${OpenCV_LIBS})
