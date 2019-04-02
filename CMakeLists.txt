cmake_minimum_required(VERSION 3.5)
project(Shimon)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Serial.h SliderGeometry.cpp SliderGeometry.h DS.h Modbus.cpp Modbus.h
        HitDelay.cpp HitDelay.h NotePosition.cpp NotePosition.h Definitions.h Striker.cpp Striker.h)

add_executable(Shimon ${SOURCE_FILES})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(BOOST_LIBS date_time system)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} -lEposCmd)