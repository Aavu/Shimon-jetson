//
// Created by Raghavasimhan Sankaranarayanan on 2019-03-21.
//

#include <string>
#include <queue>
#include <iostream>
#include <math.h>
#include <algorithm>

#ifndef MODBUS_DS_H
#define MODBUS_DS_H

using namespace std;

struct ModbusMessage {
    int armID;
    int position;
    float precision;
    int Vmax;
    float acceleration;
    int push;

    void print() {
        cout << armID << " " << position << " " << precision
             << " " << Vmax << " " << acceleration << " " << push << endl;
    }
};

struct ModbusMessageHex {
    string position;
    string precision;
    string Vmax;
    string acceleration;
    string push;

    void print() {
        cout << position << " " << precision
             << " " << Vmax << " " << acceleration << " " << push << endl;
    }
};

struct IAI_Message {
    int armID;
    int Xtarget;
    double acceleration;
    int Vmax;
    int hitVelocity;
    int arrivalTime;

    void print() {
        cout << armID << " " << Xtarget << " " << acceleration
             << " " << Vmax << " " << hitVelocity << " " << arrivalTime << endl;
    }
};

struct TimedMessage {
    int armID;
    double time;
    int Xtarget;
    double acceleration;
    int Vmax;
    int hitVelocity;
    double arrivalTime;

    void print() {
        cout << armID << " " << time << " " << Xtarget << " " << acceleration
             << " " << Vmax << " " << hitVelocity << " " << arrivalTime << endl;
    }
};

struct Interval {
    float left;
    float right;

    void print() {
        cout << "interval: " << left << " " << right << endl;
    }
};

struct Servo {
    string on = "06h 0Dh 00h 10h 00h";
    string off = "05h 04h 03h 00h 00h";
    string home = "06h 0Dh 00h 10h 10h";
    string alarmCodeQuery = "03h 90h 02h 00h 01h";
    string alarmReset = "05h 04h 07h FFh 00h";
};

#endif //MODBUS_DS_H
