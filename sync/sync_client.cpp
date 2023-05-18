#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main()
{
    io_service service;
    ip::tcp::socket sock(service);
    sock.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8001));

    const std::string msg("Hello, world!");
    write(sock, buffer(msg));
}