//
// Created by Raghavasimhan Sankaranarayanan on 2019-01-29.
//

#ifndef TEST_UDP_H
#define TEST_UDP_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <thread>
#include <iostream>
#include <sstream>
#include "DS.h"
#include "Modbus.h"
#include "SliderGeometry.h"
#include "NotePosition.h"

#define IP_ADDRESS "192.168.2.2"
#define UDP_PORT 7001

using boost::asio::ip::udp;
using boost::asio::ip::address;
using namespace std;

class UDP {
public:
    explicit UDP(boost::asio::io_context& io_context);

    IAI_Message message;
    string unicodeMessage;
    string prevUnicodeMsg;

private:
    udp::socket socket;
    std::array<char, 128> recv_buffer;
    udp::endpoint remote_endpoint;

    void splitMsg(string msg);

private:
    void handleReceive(const boost::system::error_code &error, size_t bytes_transferred);
    void startReceive();

};


#endif //TEST_UDP_H
