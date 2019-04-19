//
// Created by nvidia on 4/18/19.
//

#include "ArmController.h"

//ArmController::ArmController(int numStrikers, Serial &serial) {
//    this->serial = serial;
//    int lResult = MMC_SUCCESS;
//    if ((lResult = prepareStrikers(striker, lResult)) != MMC_SUCCESS) {
//        return lResult;
//    }
//
//    strikerMode = Normal;
//}
//
//void ArmController::listen() {
//    boost::asio::io_context io_context;
//    udp::socket socket(io_context, udp::endpoint(address::from_string(IP_ADDRESS), UDP_PORT));
//
//    while (true) {
//        boost::array<char, DATA_SIZE> recv_buf;
//        udp::endpoint remote_endpoint;
//        boost::system::error_code error;
//        auto size = (int) socket.receive_from(boost::asio::buffer(recv_buf, DATA_SIZE), remote_endpoint, 0, error);
//        string msg;
//        char id = recv_buf[0];
//        for (int i = 0; i < size; i++) {
//            msg += recv_buf[i];
//        }
//        cout << " msg: " << msg << endl;
//
//        if (msg == "quit ") {
//            cout << "Quitting..." << endl;
//
//            auto modbus = Modbus();
//            modbus.servoAxis(false);
//            for (const string &i:modbus.ccMessage) {
//                cout << i;
//                serial.write(i);
//                sleep(100);
//            }
//            break;
//        }
//
//        if (msg == "home ") {
//            cout << "Homing..." << endl;
//            if ((lResult = prepareStrikers(striker, lResult)) != MMC_SUCCESS) {
//                return lResult;
//            }
//            auto modbus = Modbus();
//            modbus.servoAxis(true);
//            for (const string &i:modbus.ccMessage) {
//                cout << i;
//                serial.write(i);
//                sleep(100);
//            }
//
//            modbus.goHome();
//            for (const string &i:modbus.ccMessage) {
//                cout << i;
//                serial.write(i);
//                sleep(100);
//            }
//
//        }
//
//        else if (msg == "on ") {
//            cout << "Switching servos on..." << endl;
//            auto modbus = Modbus();
//            modbus.servoAxis(true);
//            for (const string &i:modbus.ccMessage) {
////                    cout << i;
//                serial.write(i);
//                sleep(100);
//            }
//
//            if ((lResult = prepareStrikers(striker, lResult)) != MMC_SUCCESS) {
//                return lResult;
//            }
//
//        }
//
//        else if (msg == "off ") {
//            cout << "Switching servos off..." << endl;
//            auto modbus = Modbus();
//            modbus.servoAxis(false);
//            for (const string &i:modbus.ccMessage) {
//                cout << i;
//                serial.write(i);
//                sleep(100);
//            }
//        }
//        if (msg == "tremoloOn ") {
//            cout << "Tremolo mode..." << endl;
//            strikerMode = Tremolo;
//            thread{strike, striker[0], 10, Tremolo}.detach();
//        } else if (msg == "tremoloOff ") {
//            cout << "Normal mode..." << endl;
//            strikerMode = Normal;
//        }
////            else {
////                if (id == 'm') {
////                    auto message = splitMsg(msg);
////                    // Slider Geometry Conversion
////                    auto sliderGeometry = SliderGeometry(message);
////                    auto fMessage = sliderGeometry.convert();
////                    fMessage.print();
////                    // MODBUS conversion
////                    auto modbus = Modbus(fMessage);
////                    //send commands
////                    cout << modbus.unicodeMessage << endl;
////                    serial.write(modbus.unicodeMessage);
////                } else {
////                    int i = 0;
////                    stringstream ssin(msg);
////                    string arr[2];
////                    while (ssin.good() && i < 2) {
////                        ssin >> arr[i];
////                        ++i;
////                    }
////                    cout << "ARM ID: " << arr[0] << endl;
////                    auto armID = stoi(arr[0]);
////                    auto vel = stoi(arr[1]);
////                    if (armID == 1) {
////                        cout << armID << " " << vel << endl;
////                        thread{strike, striker[0], vel, Normal}.detach();
////                    }
////                }
////            }
//    }
//    striker[0].CloseDevice();
//}
