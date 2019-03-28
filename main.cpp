#include <stdlib.h>
#include "SliderGeometry.h"
#include "DS.h"
#include "Modbus.h"
#include "NotePosition.h"
#include "Serial.h"
#include "UDP.h"
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace std;

void sleep(int ms) {
    usleep((useconds_t) ms*1000);
}

string val = "";

int main() {
//    queue <TimedMessage> messageQueue[4];
//    TimedMessage timedMessage = {4, 642763, 624, 0.050967, 116, 80, 643228};
//
//    messageQueue[timedMessage.armID-1].push(timedMessage);
//    auto msg = messageQueue[3].front();
//    msg.print();
//

    std::thread r([&] {
        try {
            boost::asio::io_context io_context;
            UDP udp{io_context};
            io_context.run();
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }
    });

    // Slider Geometry Conversion
    IAI_Message message = {4, 157, 0.415289, 946, 80, 9896617};
    auto sliderGeometry = SliderGeometry();
    auto fMessage = sliderGeometry.convert();

    // MODBUS conversion
    auto modbus = Modbus(fMessage);

    try {
        Serial serial("/dev/ttyUSB0",230400);
        cout << "Successfully connected to Shimon's arms!" << endl;
        while(true) {
            cin >> val;
            cout << val << endl;
            if (val == "q") {
                modbus.servoAxis(false);
                for (string i:modbus.ccMessage) {
                    cout << i;
                    serial.write(i);
                    sleep(100);
                }
                break;
            }
            if (val == "home") {
                cout << "Homing..." << endl;
                modbus.servoAxis(true);
                for (string i:modbus.ccMessage) {
                    cout << i;
                    serial.write(i);
                    sleep(100);
                }
                modbus.goHome();
                for (string i:modbus.ccMessage) {
                    cout << i;
                    serial.write(i);
                    sleep(100);
                }
            }
        }

    } catch(boost::system::system_error& e)
    {
        cout << "Error: " << e.what() << endl;
        return 1;
    }

    r.join();
//    auto notePosition = NotePosition();
//    cout << notePosition.isWhiteKey(12) << endl;

    return 0;
}


