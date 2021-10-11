#include "server.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

void server::do_session(tcp::socket socket) {
    try {
        websocket::stream<tcp::socket> ws{std::move(socket)};
        ws.set_option(websocket::stream_base::decorator(
                [](websocket::response_type &res) {
                    res.set(http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-server-sync");
                }));
        ws.accept();

        do {
            boost::asio::streambuf buffer;
            ws.read(buffer);

            std::size_t n = boost::asio::read_until(socket, buffer, '\n');
            boost::asio::streambuf::const_buffers_type bufs = buffer.data();
            std::string line(
                    boost::asio::buffers_begin(bufs),
                    boost::asio::buffers_begin(bufs) + n);

            std::cerr << line << std::endl; //TODO: strip this

            std::string answer = parse(line);

            ws.write(net::buffer(std::string(answer)));
        } while (true);
    }
    catch (beast::system_error const &se) {
        if (se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }
    catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

std::string server::parse(const std::string &line) {
    std::istringstream iss(line);
    std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());
    int err = 0;

    if (results[0] == "LOAD") {
        err = l.load_resource(results[1]);
        return std::to_string(err) + "\n";
    } else if (results[0] == "GET") {
        err = l.get_value(results[1]);
        std::cerr << "returned value : " << l.value << std::endl;//TODO: strip this
        return std::to_string(err) + " " + l.value + "\n";
    } else if (results[0] == "SET") {
        err = l.set_value(results[1], results[2]);
        l.get_value(results[1]);
        std::cerr << "returned value : " << l.value << std::endl;//TODO: strip this
        return std::to_string(err) + "\n";
    } else {
        return "127\n";
    }
}


server::server() {

    l = Loader();

    try {
        auto const address = net::ip::make_address(
                "127.0.0.1"); // NOTE: it appears that "localhost" is not a valid address
        auto const port = static_cast<unsigned short>(12345);

        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, port}};

        do {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            std::thread(
                    &server::do_session, this,
                    std::move(socket)).detach();
        } while (true);
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw EXIT_FAILURE;
    }
}