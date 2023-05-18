#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

io_service service;

void handle_connections()
{
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
    char buf[1024];
    while (true)
    {
        ip::tcp::socket sock(service);
        acceptor.accept(sock);
        int bytes = sock.read_some(buffer(buf, 1024));
        std::string msg(buf, bytes);
        std::cout << msg << std::endl;
        sock.close();
    }
}

int main(int argc, char* argv[])
{
    handle_connections();
}