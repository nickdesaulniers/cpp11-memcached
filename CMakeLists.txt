cmake_minimum_required(VERSION 2.8)
add_executable(main main.cpp packet.cpp key_value_store.cpp)
add_definitions(-Wall -Wextra -O3 -std=c++11)

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)
IF (Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(main ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
ENDIF()

