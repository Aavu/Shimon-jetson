//
// Created by Raghavasimhan Sankaranarayanan on 2019-03-25.
//

#include <chrono>
#include <unistd.h>
#include "DS.h"
#include "NotePosition.h"

#ifndef SHIMON_HITDELAY_H
#define SHIMON_HITDELAY_H

class HitDelay {
public:
    unsigned int delay;
    int getSystemTime();
    int hitDelay(TimedMessage m);

};


#endif //SHIMON_HITDELAY_H
