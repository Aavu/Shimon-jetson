//
// Created by Raghavasimhan Sankaranarayanan on 2019-03-21.
//

#include "DS.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <unistd.h>

#ifndef MODBUS_MODBUS_H
#define MODBUS_MODBUS_H

using namespace std;

class Modbus {
private:
    string buffer;
    int result[25];
    int messageASCII[55];
    ModbusMessage message;
    Servo servo[4];

    string getMessageBodyInHex();
    string checksum(const string& hexValue, int a[], int b[]);
    string toHex(int i, int width);
    bool fromHex(const std::string& hexValue, int& result);
    int parseHex(const string& hexValue, int a[], int b[]);
    string parseHexToString(const string& hexValue);

public:
    string unicodeMessageBody;
    string unicodeMessage;
    string hexMessageBody;
    string hexMessage;
    string ccMessage[4];

public:
    Modbus(ModbusMessage msg = {});
    void adjust(ModbusMessage &m);
    void convertToUnicodeString(bool withChecksum = true);
    void convertToHexString();
//    void Modbus::printMessageASCII();
    void servoAxis(bool turnOn = true);
    void goHome();
};

#endif //MODBUS_MODBUS_H
