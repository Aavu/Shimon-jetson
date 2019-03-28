#include <stdlib.h>
#include "SliderGeometry.h"
#include "DS.h"
#include "Modbus.h"
#include "NotePosition.h"

using namespace std;

void sleep(int ms) {
    usleep((useconds_t) ms*1000);
}

int main() {
    queue <TimedMessage> messageQueue[4];
    TimedMessage timedMessage = {4, 642763, 624, 0.050967, 116, 80, 643228};

    messageQueue[timedMessage.armID-1].push(timedMessage);
    auto msg = messageQueue[3].front();
    msg.print();

    // Slider Geometry Conversion
    IAI_Message message = {4, 157, 0.415289, 946, 80, 9896617};
    auto sliderGeometry = SliderGeometry(message);
    auto fMessage = sliderGeometry.convert();

    fMessage.print();

    // MODBUS conversion
    auto modbus = Modbus(fMessage);
    modbus.goHome();
    for (string i:modbus.ccMessage) {
        cout << i;
        sleep(100);
    }

    auto notePosition = NotePosition();
    cout << notePosition.isWhiteKey(12) << endl;

    return 0;
}


