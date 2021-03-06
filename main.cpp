#include <cstdlib>
#include "SliderGeometry.h"
#include "DS.h"
#include "Modbus.h"
#include "NotePosition.h"
#include "Serial.h"
#include "Striker.h"
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "HitDelay.h"
//#include "ArmController.h"

#define IP_ADDRESS "192.168.2.2"
//#define IP_ADDRESS "127.0.0.1"
#define UDP_PORT 7002

#define DATA_SIZE 32

using namespace boost::asio;
using namespace std;
using namespace boost::asio::ip;

void sleep(int ms) {
    usleep((useconds_t) ms*1000);
}

IAI_Message splitMsg(const string &msg) {
    int i = 0;
    stringstream ssin(msg);
    string arr[7];
    while (ssin.good() && i < 7) {
        ssin >> arr[i];
        ++i;
    }

    IAI_Message message = {};

    message.armID = stoi(arr[1]);
    message.Xtarget = stoi(arr[2]);
    message.acceleration = stof(arr[3]);
    message.Vmax = stoi(arr[4]);
    message.hitVelocity = stoi(arr[5]);
    message.arrivalTime = stoi(arr[6]);
    return message;
}

int prepareStrikers(vector<Striker> strikers, int &lResult) {
    lResult = MMC_SUCCESS;
    unsigned int errorCode = 0;
    for (auto &s : strikers) {
        if ((lResult = s.lResult) != MMC_SUCCESS) {
            return lResult;
        } else {
            if ((lResult = s.Prepare()) != MMC_SUCCESS) {
                s.LogError("Prepare", lResult, errorCode);
                return lResult;
            }
        }
    }
    return lResult;
}

void strike(Striker striker, unsigned int m_velocity, StrikerModes mode) {
    striker.hit(m_velocity, mode);
}

StrikerModes strikerMode = Normal;

int main() {
    boost::asio::io_context io_context;

    udp::socket socket(io_context, udp::endpoint(address::from_string(IP_ADDRESS), UDP_PORT));

    try {
        Serial serial("/dev/IAIactuator", 230400);
        cout << "Successfully connected to Shimon's arms!" << endl;

        //Init Strikers...
        int lResult = MMC_SUCCESS;
        vector<Striker> striker = {Striker(0), Striker(2), Striker(3), Striker(4)};
        if ((lResult = prepareStrikers(striker, lResult)) != MMC_SUCCESS) {
            return lResult;
        }

        cout << "Successfully connected to Strikers!" << endl;
        strikerMode = Normal;

        while (true) {
            boost::array<char, DATA_SIZE> recv_buf;
            udp::endpoint remote_endpoint;
            boost::system::error_code error;
            auto size = (int) socket.receive_from(boost::asio::buffer(recv_buf, DATA_SIZE), remote_endpoint, 0, error);
            string msg;
            char id = recv_buf[0];
            for (int i = 0; i < size; i++) {
                msg += recv_buf[i];
            }
            cout << " msg: " << msg << endl;

            if (msg == "quit ") {
                cout << "Quitting..." << endl;

                auto modbus = Modbus();
                modbus.servoAxis(false);
                for (const string &i:modbus.ccMessage) {
                    cout << i;
                    serial.write(i);
                    sleep(100);
                }
                break;
            } else if (msg == "home ") {
                cout << "Homing..." << endl;
                auto modbus = Modbus();
                modbus.servoAxis(true);
                for (const string &i:modbus.ccMessage) {
                    cout << i;
                    serial.write(i);
                    sleep(100);
                }

                modbus.goHome();
                for (const string &i:modbus.ccMessage) {
                    cout << i;
                    serial.write(i);
                    sleep(100);
                }

            } else if (msg == "on ") {
                cout << "Switching servos on..." << endl;
                auto modbus = Modbus();
                modbus.servoAxis(true);
                for (const string &i:modbus.ccMessage) {
//                    cout << i;
                    serial.write(i);
                    sleep(100);
                }

            } else if (msg == "off ") {
                cout << "Switching servos off..." << endl;
                auto modbus = Modbus();
                modbus.servoAxis(false);
                for (const string &i:modbus.ccMessage) {
                    cout << i;
                    serial.write(i);
                    sleep(100);
                }
//            } else if (msg == "tremoloOn ") {
//                cout << "Tremolo mode..." << endl;
//                strikerMode = Tremolo;
//                thread{strike, striker[0], 10, Tremolo}.detach();
//            } else if (msg == "tremoloOff ") {
//                cout << "Normal mode..." << endl;
//                strikerMode = Normal;
            } else {
                if (id == 'm') {
                    auto message = splitMsg(msg);
                    // Slider Geometry Conversion
                    auto sliderGeometry = SliderGeometry(message);
                    auto fMessage = sliderGeometry.convert();
                    fMessage.print();
                    // MODBUS conversion
                    auto modbus = Modbus(fMessage);
                    //send commands
                    cout << modbus.unicodeMessage << endl;
                    serial.write(modbus.unicodeMessage);
                } else {
                    int i = 0;
                    stringstream ssin(msg);
                    string arr[2];
                    while (ssin.good() && i < 2) {
                        ssin >> arr[i];
                        ++i;
                    }
                    auto ID = stoi(arr[0]);
                    auto vel = stoi(arr[1]);
                    cout << "msg: " << ID << " " << vel << endl;
                    if (ID > 0 && ID < 4) {
                        thread{strike, striker[ID], vel, Normal}.detach();
                    }
                }
            }
        }
        striker[1].CloseDevice();
        striker[2].CloseDevice();
        striker[3].CloseDevice();
    } catch (boost::system::system_error &e) {
        cout << "Error: " << e.what() << endl;
    }
    return 0;
}


