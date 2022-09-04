cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

project(fileChecker)

find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(${PROJECT_NAME} fileChecker.c)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})

