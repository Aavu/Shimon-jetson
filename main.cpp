#include <stdlib.h>
#include "SliderGeometry.h"
#include "DS.h"
#include "Modbus.h"
#include "NotePosition.h"
#include "Serial.h"
#include "Striker.h"
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "HitDelay.h"

#define IP_ADDRESS "192.168.2.2"
#define UDP_PORT 7001

#define DATA_SIZE 32

using namespace boost::asio;
using namespace std;
using namespace boost::asio::ip;

void sleep(int ms) {
    usleep((useconds_t) ms*1000);
}

string val;

IAI_Message splitMsg(const string &msg) {
    int i = 0;
    stringstream ssin(msg);
    string arr[6];
    while (ssin.good() && i < 6) {
        ssin >> arr[i];
        ++i;
    }

    IAI_Message message = {};

    message.armID = stoi(arr[0]);
    message.Xtarget = stoi(arr[1]);
    message.acceleration = stof(arr[2]);
    message.Vmax = stoi(arr[3]);
    message.hitVelocity = (unsigned int) stoi(arr[4]);
    message.arrivalTime = stoi(arr[5]);
    return message;
}

int prepareStrikers(vector<Striker> strikers, int &lResult) {
    lResult = MMC_SUCCESS;
    for (auto &s : strikers) {
        if ((lResult = s.lResult) != MMC_SUCCESS) {
            return lResult;
        } else {
            if ((lResult = s.Prepare()) != MMC_SUCCESS) {
                s.LogError("Prepare", lResult, *s.p_pErrorCode);
                return lResult;
            }
        }
    }
    return lResult;
}

void strike(Striker striker, unsigned int delay, unsigned int m_velocity, int mode = 0) {
    striker.sleep_ms(delay);
    striker.hit(m_velocity, mode);
}

int main() {
    boost::asio::io_context io_context;

    //Init Strikers...
    int lResult = MMC_FAILED;
    vector<Striker> striker = {Striker(0, 0)};
    thread executeStriker[striker.size()];

    unsigned int delay = 465;

    if ((lResult = prepareStrikers(striker, lResult)) != MMC_SUCCESS) {
        return lResult;
    }

    udp::socket socket(io_context, udp::endpoint(address::from_string(IP_ADDRESS), UDP_PORT));

    try {
        Serial serial("/dev/ttyUSB0", 230400);
        cout << "Successfully connected to Shimon's arms!" << endl;

        while (true) {
            boost::array<char, DATA_SIZE> recv_buf;
            udp::endpoint remote_endpoint;
            boost::system::error_code error;
            auto size = socket.receive_from(boost::asio::buffer(recv_buf, DATA_SIZE), remote_endpoint, 0, error);
            string msg;
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

                if ((lResult = prepareStrikers(striker, lResult)) != MMC_SUCCESS) {
                    return lResult;
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

                if ((lResult = prepareStrikers(striker, lResult)) != MMC_SUCCESS) {
                    return lResult;
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
                //striker
                auto armID = HitDelay().hitDelay(message);
                //send commands
                cout << modbus.unicodeMessage << endl;
                serial.write(modbus.unicodeMessage);
                //For now as we have only 1 motor
                if (armID < striker.size()) {
                    executeStriker[armID] = thread(strike, striker[armID], delay, message.hitVelocity);
                }
            }
        }

    } catch (boost::system::system_error &e) {
        cout << "Error: " << e.what() << endl;
    }

    for (int i = 0; i < striker.size(); i++) {
        executeStriker[i].join();
        striker[i].CloseDevice();
    }

    return 0;
}


