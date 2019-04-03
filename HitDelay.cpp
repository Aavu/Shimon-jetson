//
// Created by Raghavasimhan Sankaranarayanan on 2019-03-25.
//

#include "HitDelay.h"

int HitDelay::hitDelay(IAI_Message m) {
//    auto delayTime = (unsigned int) m.arrivalTime - getSystemTime();
//    delayTime = 465;
    auto notePosition = NotePosition();
//    usleep(delayTime);
    if (notePosition.isWhiteKey(m.Xtarget)) {
        return (((m.armID - 1) * 2) + 1);
    } else {
        return ((m.armID - 1) * 2);
    }
}

int HitDelay::getSystemTime() {
    return (int) std::chrono::system_clock::now().time_since_epoch().count()/1000;
}

