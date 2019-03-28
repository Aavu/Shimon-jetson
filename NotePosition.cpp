//
// Created by Raghavasimhan Sankaranarayanan on 2019-03-25.
//

#include "NotePosition.h"

int NotePosition::getNote(int value) {
    auto it = std::find(notes.begin(), notes.end(), value);
    if (it != notes.end()) return (int) std::distance(notes.begin(), it);
    else return -1;
}

int NotePosition::notePosition(int value) {
    int val = getNote(value) + 48;
    if (val < 97 && val > 47) {
        return val;
    } else {
        return -1;
    }
}

bool NotePosition::isWhiteKey(int keyValue) {
    int val = notePosition(keyValue)%12;
    for (int i : whiteKeys) {
        if (val == i) {
            return true;
        }
    }
    return false;
}