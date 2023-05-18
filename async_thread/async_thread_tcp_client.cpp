#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

using namespace boost::asio;

io_service service;
ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8080);
ip::tcp::socket sock(service);
char read_buf[128];

void on_write(const boost::system::error_code& err, size_t bytes)
{
    std::cout << "message sent to server: " << bytes << "bytes" << std::endl;
}

void on_connect(const boost::system::error_code& err)
{
    async_write(sock, buffer("Hello from client!\n", 20), boost::bind(on_write, boost::placeholders::_1, boost::placeholders::_2));
}

int main(int argc, char* argv[])
{
    int count_req = 1024;
    if (argc > 1)
    {
        count_req = atoi(argv[1]);
    }

    boost::this_thread::sleep(boost::posix_time::millisec(500));

    for (int req = 0; req < count_req; ++req)
    {
        sock.async_connect(ep, boost::bind(on_connect, boost::placeholders::_1));
    }

    service.run();
}