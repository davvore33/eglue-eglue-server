//
// Created by matteo on 10/10/21.
//

#ifndef EGLUE_SERVER_SERVER_H
#define EGLUE_SERVER_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include "library.h"

class server {
public:
    server();

//void do_session(boost::asio::ip::tcp::socket socket);
private:
    Loader l;

    void do_session(boost::asio::ip::tcp::socket socket);

    std::string parse(const std::string &line);
};

#endif //EGLUE_SERVER_SERVER_H
