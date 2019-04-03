#include <stdlib.h>
#include "SliderGeometry.h"
#include "DS.h"
#include "Modbus.h"
#include "NotePosition.h"
#include "Serial.h"
#include "Striker.h"
#include <boost/asio.hpp>
#include <boost/array.hpp>

#define IP_ADDRESS "192.168.2.2"
#define UDP_PORT 7001

using namespace boost::asio;
using namespace std;
using namespace boost::asio::ip;

void sleep(int ms) {
    usleep((useconds_t) ms*1000);
}

string val = "";

IAI_Message splitMsg(string msg) {
    int i = 0;
    stringstream ssin(msg);
    string arr[6];
    while (ssin.good() && i < 6) {
        ssin >> arr[i];
        ++i;
    }

    IAI_Message message = {};

    message.armID = stoi(arr[0].c_str());
    message.Xtarget = stoi(arr[1].c_str());
    message.acceleration = stof(arr[2].c_str());
    message.Vmax = stoi(arr[3].c_str());
    message.hitVelocity = stoi(arr[4].c_str());
    message.arrivalTime = stoi(arr[5].c_str());
    return message;
}

void strike(Striker striker, int dly, int m_velocity, int mode) {
    striker.sleep_ms(dly);
    cout << "inside " << striker.getID() << endl;
    striker.hit(m_velocity, mode);
    striker.sleep_ms(dly);
    striker.hit(m_velocity, mode);
    striker.sleep_ms(dly);
    striker.hit(m_velocity, mode);
    striker.sleep_ms(dly);
    striker.hit(m_velocity, mode);
    striker.sleep_ms(dly);
}

int main() {
    boost::asio::io_context io_context;

    //Init Strikers...
    int lResult = MMC_FAILED;
    vector<Striker> striker = {Striker(0, 0)};
    thread executeStriker[striker.size()];
    int delay = 100;
    for (auto &s : striker) {
        if ((lResult = s.lResult) != 0) {
            return lResult;
        } else {
            if ((lResult = s.Prepare()) != 0) {
                s.LogError("Prepare", lResult, *s.p_pErrorCode);
                if (s.setCurrent(0) == 0) {
                    s.LogError("setCurrent", lResult, *s.p_pErrorCode);
                    lResult = MMC_FAILED;
                    return lResult;
                }
                return lResult;
            }

            executeStriker[0] = thread(strike, striker[0], delay, 127, 0);
        }
    }



//    udp::socket socket(io_context, udp::endpoint(address::from_string(IP_ADDRESS), UDP_PORT));
//
//    try {
//        Serial serial("/dev/ttyUSB0",230400);
//        cout << "Successfully connected to Shimon's arms!" << endl;
//
//        while(true) {
//            boost::array<char, 32> recv_buf;
//            udp::endpoint remote_endpoint;
//            boost::system::error_code error;
//            int size = socket.receive_from(boost::asio::buffer(recv_buf, 32), remote_endpoint, 0, error);
//            string msg = "";
//            for (int i = 0; i < size; i++) {
//                msg += recv_buf[i];
//            }
//            cout << msg << " size: " << size << endl;
//
//            if (msg == "quit ") {
//                cout << "Quitting..." << endl;
//                auto modbus = Modbus();
//                modbus.servoAxis(false);
//                for (string i:modbus.ccMessage) {
//                    cout << i;
//                    serial.write(i);
//                    sleep(100);
//                }
//                break;
//            } else if (msg == "home ") {
//                cout << "Homing..." << endl;
//                auto modbus = Modbus();
//                modbus.servoAxis(true);
//                for (string i:modbus.ccMessage) {
//                    cout << i;
//                    serial.write(i);
//                    sleep(100);
//                }
//                modbus.goHome();
//                for (string i:modbus.ccMessage) {
//                    cout << i;
//                    serial.write(i);
//                    sleep(100);
//                }
//            } else if (msg == "on ") {
//                cout << "Switching servos on..." << endl;
//                auto modbus = Modbus();
//                modbus.servoAxis(true);
//                for (string i:modbus.ccMessage) {
//                    cout << i;
//                    serial.write(i);
//                    sleep(100);
//                }
//            } else if (msg == "off ") {
//                cout << "Switching servos off..." << endl;
//                auto modbus = Modbus();
//                modbus.servoAxis(false);
//                for (string i:modbus.ccMessage) {
//                    cout << i;
//                    serial.write(i);
//                    sleep(100);
//                }
//            } else {
//                auto message = splitMsg(msg);
//                // Slider Geometry Conversion
//                auto sliderGeometry = SliderGeometry(message);
//                auto fMessage = sliderGeometry.convert();
//                fMessage.print();
//                // MODBUS conversion
//                auto modbus = Modbus(fMessage);
//                cout << modbus.unicodeMessage << endl;
//                serial.write(modbus.unicodeMessage);
//            }
//        }
//
//    } catch(boost::system::system_error& e) {
//        cout << "Error: " << e.what() << endl;
//    }

    executeStriker[0].join();
    striker[0].CloseDevice();
    return 0;
}


