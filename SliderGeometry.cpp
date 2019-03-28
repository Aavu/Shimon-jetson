//
// Created by Raghavasimhan Sankaranarayanan on 2019-03-21.
//

#include "SliderGeometry.h"

SliderGeometry::SliderGeometry(IAI_Message msg) {
    message = msg;
}

ModbusMessage SliderGeometry::convert() {
    ModbusMessage fMessage = {};
    fMessage.armID = message.armID - 1;
    fMessage.precision = 0.1;
    fMessage.acceleration = this->message.acceleration;
    fMessage.Vmax = message.Vmax;
    fMessage.push = 0;
    fMessage.position = subtract(message.Xtarget, fMessage.armID);
    return fMessage;
}

int SliderGeometry::subtract(int value, int armID) {
    switch (armID) {
        case 1:
            return value - 40;

        case 2:
            return 1350 - value;

        case 3:
            return 1385 - value;

        default:
            return value;
    }
}
