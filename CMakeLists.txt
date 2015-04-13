cmake_minimum_required(VERSION 2.8)
add_executable(main hello.cpp packet.cpp key_value_store.cpp)
add_definitions(-Wall -Wextra -O3 -std=c++11)

FIND_PACKAGE(Boost COMPONENTS system)
IF (Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(main ${Boost_LIBRARIES} pthread)
ENDIF()

