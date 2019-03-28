cmake_minimum_required(VERSION 3.5)
project(Shimon)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Serial.h UDP.cpp UDP.h SliderGeometry.cpp SliderGeometry.h DS.h Modbus.cpp Modbus.h HitDelay.cpp HitDelay.h NotePosition.cpp NotePosition.h)

add_executable(Shimon ${SOURCE_FILES})

set(BOOST_LIBS date_time system)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
target_link_libraries(Shimon ${Boost_LIBRARIES})
find_package(Threads REQUIRED)
target_link_libraries(Shimon ${CMAKE_THREAD_LIBS_INIT})