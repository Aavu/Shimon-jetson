#include <stdlib.h>
#include "SliderGeometry.h"
#include "DS.h"
#include "Modbus.h"
#include "NotePosition.h"
#include "Serial.h"
#include "Striker.h"
//#include "UDP.h"
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

int main() {
    boost::asio::io_context io_context;
    int lResult = MMC_FAILED;
    unsigned int ulErrorCode = 0;
    vector<Striker> striker = {Striker(0, 0), Striker(0, 1)};
    for (int i = 0; i < striker.size(); i++) {
        if ((lResult = striker[i].lResult) != MMC_SUCCESS) {
            return lResult;
        } else {
            ulErrorCode = striker[i].ulErrorCode;
            if ((lResult = striker[i].Prepare(&ulErrorCode)) != MMC_SUCCESS) {
                striker[i].LogError("Prepare", lResult, ulErrorCode);
                return lResult;
            }
        }
    }

//    UDP udp{io_context};
//    std::thread r([&] {
//        try {
//            io_context.run();
//        } catch (const std::exception& ex) {
//            std::cerr << ex.what() << std::endl;
//        }
//    });

    udp::socket socket(io_context, udp::endpoint(address::from_string(IP_ADDRESS), UDP_PORT));
//    while (true) {
//        boost::array<char, 32> recv_buf;
//        udp::endpoint remote_endpoint;
//        boost::system::error_code error;
//        int size = socket.receive_from(boost::asio::buffer(recv_buf, 32), remote_endpoint, 0, error);
//        string msg = "";
//        for (int i = 0; i < size; i++) {
//            msg += recv_buf[i];
//        }
//        cout << msg << " size: " << size << endl;
//        if (msg == "quit ") {
//            cout << "Quitting..." << endl;
//            break;
//        } else if (msg == "home ") {
//            cout << "Homing..." << endl;
//        } else {
//            auto message = splitMsg(msg);
//            // Slider Geometry Conversion
//            auto sliderGeometry = SliderGeometry(message);
//            auto fMessage = sliderGeometry.convert();
//            // MODBUS conversion
//            auto modbus = Modbus(fMessage);
//            cout << modbus.unicodeMessage << endl;
//        }
//
//    }


    try {
        Serial serial("/dev/ttyUSB0",230400);
        cout << "Successfully connected to Shimon's arms!" << endl;


        while(true) {
            boost::array<char, 32> recv_buf;
            udp::endpoint remote_endpoint;
            boost::system::error_code error;
            int size = socket.receive_from(boost::asio::buffer(recv_buf, 32), remote_endpoint, 0, error);
            string msg = "";
            for (int i = 0; i < size; i++) {
                msg += recv_buf[i];
            }
            cout << msg << " size: " << size << endl;

            if (msg == "quit ") {
                cout << "Quitting..." << endl;
                auto modbus = Modbus();
                modbus.servoAxis(false);
                for (string i:modbus.ccMessage) {
                    cout << i;
                    serial.write(i);
                    sleep(100);
                }
                break;
            } else if (msg == "home ") {
                cout << "Homing..." << endl;
                auto modbus = Modbus();
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
            } else if (msg == "on ") {
                cout << "Switching servos on..." << endl;
                auto modbus = Modbus();
                modbus.servoAxis(true);
                for (string i:modbus.ccMessage) {
                    cout << i;
                    serial.write(i);
                    sleep(100);
                }
            } else if (msg == "off ") {
                cout << "Switching servos off..." << endl;
                auto modbus = Modbus();
                modbus.servoAxis(false);
                for (string i:modbus.ccMessage) {
                    cout << i;
                    serial.write(i);
                    sleep(100);
                }
            } else {
                auto message = splitMsg(msg);
                // Slider Geometry Conversion
                auto sliderGeometry = SliderGeometry(message);
                auto fMessage = sliderGeometry.convert();
                fMessage.print();
                // MODBUS conversion
                auto modbus = Modbus(fMessage);
                cout << modbus.unicodeMessage << endl;
                serial.write(modbus.unicodeMessage);
            }


//            if (udp.unicodeMessage != udp.prevUnicodeMsg) {
//                if (udp.unicodeMessage == "quit ") {
//                    cout << "quitting" << endl;
////                    modbus.servoAxis(false);
////                    for (string i:modbus.ccMessage) {
////                        cout << i;
////                        serial.write(i);
////                        sleep(100);
////                    }
//                    break;
//                }
//                if (udp.unicodeMessage == "home ") {
//                    cout << "Homing..." << endl;
////                    modbus.servoAxis(true);
////                    for (string i:modbus.ccMessage) {
////                        cout << i;
////                        serial.write(i);
////                        sleep(100);
////                    }
////                    modbus.goHome();
////                    for (string i:modbus.ccMessage) {
////                        cout << i;
////                        serial.write(i);
////                        sleep(100);
////                    }
//                } else {
//                    cout << udp.unicodeMessage << endl;
////                serial.write(udp.unicodeMessage);
//                }
//            }
//            udp.prevUnicodeMsg = udp.unicodeMessage;
//        }
        }

    } catch(boost::system::system_error& e)
    {
        cout << "Error: " << e.what() << endl;
        return 1;
    }
//
//    r.join();
////    auto notePosition = NotePosition();
////    cout << notePosition.isWhiteKey(12) << endl;

    return 0;
}


