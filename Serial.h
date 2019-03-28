//
// Created by Raghavasimhan Sankaranarayanan on 2019-01-29.
//

#ifndef TEST_SERIAL_H
#define TEST_SERIAL_H

#include <boost/asio.hpp>
#include <iostream>

using namespace std;
using namespace boost;

class Serial {
public:
    Serial(string device, unsigned int baud_rate) : io(), serial(io, device) {
        serial.set_option(asio::serial_port_base::baud_rate(baud_rate));
    }

    void write(string message) {
        asio::write(serial, asio::buffer(message.c_str(), message.size()));
    }

    string readline() {
        char c;
        std::string result;
        for(;;)
        {
            asio::read(serial,asio::buffer(&c,1));
            switch(c)
            {
                case '\r':
                    break;
                case '\n':
                    return result;
                default:
                    result+=c;
            }
        }
    }

private:
    asio::io_context io;
    asio::serial_port serial;
};


#endif //TEST_SERIAL_H
