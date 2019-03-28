cmake_minimum_required(VERSION 3.13)
project(Shimon)

set(CMAKE_CXX_STANDARD 14)

add_executable(Shimon main.cpp SliderGeometry.cpp SliderGeometry.h DS.h Modbus.cpp Modbus.h HitDelay.cpp HitDelay.h NotePosition.cpp NotePosition.h)