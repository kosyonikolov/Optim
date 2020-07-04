cmake_minimum_required(VERSION 3.0.0)
project(Optim VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

include_directories(inc/ ${OpenCV_INCLUDES})

add_executable(Optim src/main.cpp src/common.cpp src/Genocop.cpp)

target_link_libraries(Optim ${OpenCV_LIBS})
target_compile_options(Optim PRIVATE -Wall -Wextra)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
