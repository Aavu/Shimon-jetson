//
// Created by Raghavasimhan Sankaranarayanan on 2019-03-21.
//

#include "Modbus.h"

Modbus::Modbus(ModbusMessage msg) {
    this->message = msg;
    convertToUnicodeString();
    cout << "here" << endl;
}

void Modbus::servoAxis(bool turnOn) {
    int r[4][6];
    int rASCII[4][17];

    for (int i=0; i < 4; i++) {
        buffer = "0" + to_string(i+1) + "h ";
        if (turnOn) buffer += servo[i].on;
        else buffer += servo[i].off;
        string sum = checksum(buffer, r[i], rASCII[i]);
        buffer += " " + sum + "h";
        for (int j = 0; j < 2; j++) rASCII[i][13+j] = toupper(sum[j]);
        rASCII[i][15] = 13;
        rASCII[i][16] = 10;
        ccMessage[i] = parseHexToString(buffer);
    }
}

void Modbus::goHome() {
    int r[4][6];
    int rASCII[4][17];
    for (int i=0; i < 4; i++) {
        buffer = "0" + to_string(i+1) + "h " + servo[i].home;
        string sum = checksum(buffer, r[i], rASCII[i]);
        buffer += " " + sum + "h";
        for (int j = 0; j < 2; j++) rASCII[i][13+j] = toupper(sum[j]);
        rASCII[i][15] = 13;
        rASCII[i][16] = 10;
        ccMessage[i] = parseHexToString(buffer);
    }
}

void Modbus::convertToHexString() {
    adjust(message);
    buffer = getMessageBodyInHex();
    hexMessageBody = buffer;
    string sum = checksum(buffer, result, messageASCII);
    buffer += " " + sum + "h";
    hexMessage = buffer;
    for (int i = 0; i < 2; i++) messageASCII[51+i] = toupper(sum[i]);
    messageASCII[53] = 13;
    messageASCII[54] = 10;
}

void Modbus::convertToUnicodeString(bool withChecksum) {
    if (hexMessage.empty()) convertToHexString();
    if (!withChecksum) unicodeMessageBody = parseHexToString(hexMessageBody);
    else unicodeMessage = parseHexToString(hexMessage);
}

//void Modbus::printMessageASCII() {
//    for (int i : messageASCII) {
//        cout << i << " ";
//    }
//    cout << endl;
//}

string Modbus::checksum(const string& hexValue, int a[], int b[]) {
    return toHex(256 - parseHex(hexValue, a, b)%256, 2);
}

void Modbus::adjust(ModbusMessage &m) {
    m.acceleration *= 100;
    m.acceleration += 0.5;
    if (m.acceleration < 1) m.acceleration = 1;
    m.Vmax = max(m.Vmax * 100, 1);
    m.position *= 100;
    m.precision *= 100;
}

string Modbus::toHex(int i, int width) {
    stringstream stream;
    stream << setfill ('0') << setw(width) << hex << i;
    return stream.str();
}

bool Modbus::fromHex(const string& hexValue, int& result) {
    stringstream ss;
    ss << hex << hexValue;
    ss >> result;
    return !ss.fail();
}

int Modbus::parseHex(const string& hexValue, int a[], int b[]) {
    int value = 0;
    string s_val;
    int idx = 0;
    int idx1 = 1;
    int sum = 0;
    b[0] = 58; // add ':'
    for (char c : hexValue) {
        if (c == ' ') {
            value = 0;
            s_val = "";
        } else if (c == 'h') {
            fromHex(s_val, value);
            a[idx] = value;
            idx++;
            sum += value;
        } else {
            s_val += c;
            b[idx1] = toupper(c);
            idx1++;
        }
    }
    return sum;
}

string Modbus::parseHexToString(const string& hexValue) {
    string s_val;
    for (char c : hexValue) {
        if (c != ' ' && c != 'h') {
            s_val += toupper(c);
        }
    }
    s_val = ":"+s_val+"\r\n";
    return s_val;
}

string Modbus::getMessageBodyInHex() {
    ModbusMessageHex msg = {};
    msg.acceleration = toHex((int)message.acceleration, 4);
    msg.position = toHex(message.position, 8);
    msg.Vmax = toHex(message.Vmax, 6);
    msg.precision = toHex((int)message.precision, 4);
    msg.push = toHex(message.push, 4);

    string tpW[4], pbW[2], vW[3], aW[2], pW[2];

    for (int i = 0; i < 8; i++) {
        tpW[i/2] += msg.position[i];
        if (i < 6) vW[i/2] += msg.Vmax[i];
        if (i < 4) {
            pbW[i/2] += msg.precision[i];
            aW[i/2] += msg.acceleration[i];
            pW[i/2] += msg.push[i];
        }
    }

    char c_axis[2];
    sprintf(c_axis, "%.2X", message.armID + 1);
    return (string) c_axis + "h " + "10h 99h 00h 00h 09h 12h " + tpW[0] + "h " + tpW[1] + "h " + tpW[2] + "h " + tpW[3] + "h "
           + "00h 00h " + pbW[0] + "h " + pbW[1] + "h " + "00h " + vW[0] + "h " + vW[1] + "h " + vW[2] + "h "
           + aW[0] + "h " + aW[1] + "h " + pW[0] + "h " + pW[1] + "h " + "00h 00h";
}
