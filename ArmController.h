//
// Created by nvidia on 4/18/19.
//

#ifndef SHIMON_ARMCONTROLLER_H
#define SHIMON_ARMCONTROLLER_H

#include <cstdlib>
#include "SliderGeometry.h"
#include "DS.h"
#include "Modbus.h"
#include "NotePosition.h"
#include "Serial.h"
#include "Striker.h"
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "HitDelay.h"

#define DATA_SIZE 32

using namespace boost::asio;
using namespace std;
using namespace boost::asio::ip;

class ArmController {
//private:
//    Serial serial;
//    vector<Striker> striker = {Striker(0, 0)};
//
//public:
//    ArmController(int numStrikers, Serial &serial);
//    void listen();

};


#endif //SHIMON_ARMCONTROLLER_H
