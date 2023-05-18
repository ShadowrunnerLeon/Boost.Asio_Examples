#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

using namespace boost::asio;

io_service service;
ip::udp::endpoint ep(ip::address::from_string("127.0.0.1"), 8080);
ip::udp::socket sock(service, ip::udp::endpoint(ip::udp::v4(), 0));
char read_buf[20];

void on_send(const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
        std::cout << "message sent to server: " << bytes << "bytes" << std::endl;
    }
    else
    {
        std::cerr << "on_send: " << err << std::endl;
    }
}

void on_recv(const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
        std::string msg(read_buf, bytes);
        std::cout << "message from server: " << msg << std::endl;
    }
    else
    {
        std::cerr << "on_recv: " << err << std::endl;
    }
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
        sock.async_send_to(buffer("message from client"), ep,
            boost::bind(on_send, boost::placeholders::_1, boost::placeholders::_2));

        sock.async_receive_from(buffer(read_buf, 19), ep,
            boost::bind(on_recv, boost::placeholders::_1, boost::placeholders::_2));
    }

    service.run();
}