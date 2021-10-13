//
// Created by matteo on 10/10/21.
//

#ifndef EGLUE_SERVER_MY_SERVER_H
#define EGLUE_SERVER_MY_SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "library.h"

class context;

class my_server {
public:
    my_server();

    ~my_server() {};

    static void my_kill(int sig_num);

private:

    void do_session(tcp::socket socket, const context *my_context);

    std::string parse(const std::string &line);

private:
    Loader m_loader;

};


#endif //EGLUE_SERVER_MY_SERVER_H
