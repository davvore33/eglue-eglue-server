#include "my_server.h"

std::mutex my_mutex;

struct context {
public:
    void get(int &out) const {
        std::lock_guard<std::mutex> guard(my_mutex);
        out = stop;
    }

    void set(bool in) {
        std::lock_guard<std::mutex> guard(my_mutex);
        stop = in;
    }

private:
    bool stop = false;
};

context my_context;

void my_server::do_session(tcp::socket socket, const context *in_context) {
    try {
        websocket::stream<tcp::socket> ws{std::move(socket)};
        ws.set_option(websocket::stream_base::decorator(
                [](websocket::response_type &res) {
                    res.set(http::field::server,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-server-sync");
                }));
        ws.accept();

        int quit = false;
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

            in_context->get(quit);
        } while (!quit);

//      Received SIGINT we proceed to clean stream and socket resources
        ws.close(boost::beast::websocket::close_code::normal);
        socket.close();
        return;
    }
    catch (beast::system_error const &se) {
        if (se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }
    catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

std::string my_server::parse(const std::string &line) {
    std::istringstream iss(line);
    std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
                                     std::istream_iterator<std::string>());
    int err = 0;

    if (results[0] == "LOAD") {
        err = m_loader.load_resource(results[1]);
        return std::to_string(err) + "\n";
    } else if (results[0] == "GET") {
        err = m_loader.get_value(results[1]);
        if (!err) {
            std::cerr << "returned value : " << m_loader.value << std::endl;//TODO: strip this
            return std::to_string(err) + " " + m_loader.value + "\n";
        }
        return std::to_string(err) + " " + m_loader.value + "\n";
    } else if (results[0] == "SET") {
        err = m_loader.set_value(results[1], results[2]);
        if (!err) {
            m_loader.get_value(results[1]);
            std::cerr << "returned value : " << m_loader.value << std::endl;//TODO: strip this
        }
        return std::to_string(err) + "\n";
    } else {
        return "127\n";
    }
}

my_server::my_server() {

    m_loader = Loader();
    my_context = context();
    my_context.set(false);

    try {
        auto const address = net::ip::make_address(
                "127.0.0.1"); // NOTE: it appears that "localhost" is not a valid address
        auto const port = static_cast<unsigned short>(12345);

        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, port}};

        int quit = false;
        do {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            std::thread(
                    &my_server::do_session, this,
                    std::move(socket), &my_context).detach();
            my_context.get(quit);
        } while (!quit);
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw e;
    }
}


void my_server::signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";

    my_context.set(true);
    exit(0);
}
