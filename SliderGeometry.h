//
// Created by Raghavasimhan Sankaranarayanan on 2019-03-21.
//

#ifndef MODBUS_CONVERTSLIDERGEOMETRY_H
#define MODBUS_CONVERTSLIDERGEOMETRY_H

#include "DS.h"

class SliderGeometry {

private:
    IAI_Message message;
    int subtract(int value, int armID);

public:
    explicit SliderGeometry(IAI_Message message);
    ModbusMessage convert();
};


#endif //MODBUS_CONVERTSLIDERGEOMETRY_H
