//
// Created by Raghavasimhan Sankaranarayanan on 2019-01-29.
//

#include "UDP.h"

void UDP::handleReceive(const boost::system::error_code &error, size_t bytes_transferred) {
    if (error) {
        std::cout << "Receive failed: " << error.message() << "\n";
        return;
    }
    std::string msg = std::string(recv_buffer.begin(), recv_buffer.begin()+bytes_transferred);
    cout << msg << endl;
    startReceive();
}

void UDP::startReceive() {
    socket.async_receive_from(boost::asio::buffer(recv_buffer),
                              remote_endpoint,
                              boost::bind(&UDP::handleReceive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

UDP::UDP(boost::asio::io_context& io_context) : socket(io_context) {
    socket.open(udp::v4());
    socket.bind(udp::endpoint(address::from_string(IP_ADDRESS), UDP_PORT));
    startReceive();
}