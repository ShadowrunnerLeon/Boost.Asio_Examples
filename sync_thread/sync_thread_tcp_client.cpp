#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main()
{
    io_service service;
    ip::tcp::socket sock(service);
    sock.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8001));

    char buf[20];

    for (int i = 0; i < 1024; ++i)
    {
        int bytes_to = sock.write_some(buffer("Hello, world!", 14));
        std::cout << "message sent to client: " << bytes_to << " bytes" << std::endl;

        int bytes_from = sock.read_some(buffer(buf, 20));
        std::string msg_from_client(buf, bytes_from);
        std::cout << "message from client: " << msg_from_client << std::endl;
    }
}