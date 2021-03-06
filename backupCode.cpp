int axis = fMessage.armID;
int targetPosition = fMessage.position;
float positioningBand = fMessage.precision;
int velocity = fMessage.Vmax;
float acceleration = fMessage.acceleration;
int push = fMessage.push;

acceleration *= 100;
velocity *= 100;
acceleration += 0.5;
if(acceleration < 1) acceleration = 1;
if(velocity < 1) velocity = 1;

char c_axis[2];
sprintf(c_axis, "%.2X", axis+1);
string buffer;
string targetPositionHex = toHex(targetPosition*100, 8);
string positioningBandHex = toHex((int)(positioningBand*100), 4);
string velocityHex = toHex(velocity, 6);
string accelerationHex = toHex((int)acceleration, 4);
string pushHex = toHex(push, 4);
string tpW[4], pbW[2], vW[3], aW[2], pW[2];

for (int i = 0; i < 8; i++) {
tpW[i/2] += targetPositionHex[i];
if (i < 6) vW[i/2] += velocityHex[i];
if (i < 4) {
pbW[i/2] += positioningBandHex[i];
aW[i/2] += accelerationHex[i];
pW[i/2] += pushHex[i];
}
}

int* result = new int[25];
int* resultASCII = new int[55];
buffer = (string) c_axis + "h " + "10h 99h 00h 00h 09h 12h " + tpW[0] + "h " + tpW[1] + "h " + tpW[2] + "h " + tpW[3] + "h "
         + "00h 00h " + pbW[0] + "h " + pbW[1] + "h " + "00h " + vW[0] + "h " + vW[1] + "h " + vW[2] + "h "
         + aW[0] + "h " + aW[1] + "h " + pW[0] + "h " + pW[1] + "h " + "00h 00h";
cout << buffer << endl;

string sum = toHex(256 - parseHex(buffer, result, resultASCII)%256, 2);
resultASCII[53] = 13;
resultASCII[54] = 10;
buffer += " " + sum + "h";
cout << buffer << endl;
for (int i = 0; i < 2; i++) {
resultASCII[51+i] = (int)sum[i];
}
for(int i=0; i < 55; i++) {
cout << resultASCII[i] << " ";
}
cout << endl << endl;
string s = ":041099000009120001DFB00000000A00017188002A000000007A\r\n";
cout << parseHexToString(buffer) << endl;
cout << (parseHexToString(buffer) == s) << endl;
delete[] result;
delete[] resultASCII;

string toHex(int i, int width) {
    stringstream stream;
    stream << setfill ('0') << setw(width) << hex << i;
    return stream.str();
}

bool fromHex(const string& hexValue, int& result) {
    stringstream ss;
    ss << hex << hexValue;
    ss >> result;
    return !ss.fail();
}

int parseHex(const string& hexValue, int a[], int b[]) {
    int value = 0;
    string s_val = "";
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

string parseHexToString(const string& hexValue) {
    string s_val = "";
    for (char c : hexValue) {
        if (c != ' ' && c != 'h') {
            s_val += toupper(c);
        }
    }
    s_val = ":"+s_val+"\r\n";
    return s_val;
}


UDP udp{io_context};
std::thread r([&] {
    try {
        io_context.run();
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
    }
});

while (true) {
boost::array<char, 32> recv_buf;
udp::endpoint remote_endpoint;
boost::system::error_code error;
int size = socket.receive_from(boost::asio::buffer(recv_buf, 32), remote_endpoint, 0, error);
string msg = "";
for (
int i = 0;
i<size;
i++) {
msg += recv_buf[i];
}
cout << msg << " size: " << size <<
endl;
if (msg == "quit ") {
cout << "Quitting..." <<
endl;
break;
} else if (msg == "home ") {
cout << "Homing..." <<
endl;
} else {
auto message = splitMsg(msg);
// Slider Geometry Conversion
auto sliderGeometry = SliderGeometry(message);
auto fMessage = sliderGeometry.convert();
// MODBUS conversion
auto modbus = Modbus(fMessage);
cout << modbus.unicodeMessage <<
endl;
}

}

if (udp.unicodeMessage != udp.prevUnicodeMsg) {
if (udp.unicodeMessage == "quit ") {
cout << "quitting" <<
endl;
//                    modbus.servoAxis(false);
//                    for (string i:modbus.ccMessage) {
//                        cout << i;
//                        serial.write(i);
//                        sleep(100);
//                    }
break;
}
if (udp.unicodeMessage == "home ") {
cout << "Homing..." <<
endl;
//                    modbus.servoAxis(true);
//                    for (string i:modbus.ccMessage) {
//                        cout << i;
//                        serial.write(i);
//                        sleep(100);
//                    }
//                    modbus.goHome();
//                    for (string i:modbus.ccMessage) {
//                        cout << i;
//                        serial.write(i);
//                        sleep(100);
//                    }
} else {
cout << udp.unicodeMessage <<
endl;
//                serial.write(udp.unicodeMessage);
}
}
udp.
prevUnicodeMsg = udp.unicodeMessage;

r.

join();

auto notePosition = NotePosition();
cout << notePosition.isWhiteKey(12) <<
endl;