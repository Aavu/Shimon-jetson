//
// Created by Raghavasimhan Sankaranarayanan on 2019-03-25.
//

#ifndef SHIMON_NOTEPOSITION_H
#define SHIMON_NOTEPOSITION_H

#include <vector>

using namespace std;

class NotePosition {
private:
    vector <int> notes = {0, 10, 44, 73, 102, 157, 184, 212, 240, 267, 294, 324,
                                 377, 406, 434, 463, 490, 546, 574, 599, 624, 651, 673, 698,
                                 749, 771, 798, 820, 846, 894, 919, 945, 969, 993, 1018, 1044,
                                 1092, 1118, 1142, 1167, 1193, 1240, 1266, 1291, 1315, 1339, 1364, 1385, 1385};

    int whiteKeys[5] = {1, 3, 6, 8, 10};


public:
    int getNote(int value);
    int notePosition(int value);
    bool isWhiteKey(int keyValue);
};


#endif //SHIMON_NOTEPOSITION_H
