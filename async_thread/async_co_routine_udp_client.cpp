#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/yield.hpp>

using namespace boost::asio;

io_service service;

class server_connection : public coroutine
{
    private:
        ip::udp::endpoint ep;
        ip::udp::socket sock;
        char read_buf[20];

    private:
        void on_complete(const boost::system::error_code& err, size_t bytes)
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

    public:
        server_connection() : ep(ip::address::from_string("127.0.0.1"), 8080),
            sock(service, ip::udp::endpoint(ip::udp::v4(), 0)) {}

        void coro_function(const boost::system::error_code& err = boost::system::error_code(), size_t bytes = 0)
        {
            reenter(this)
            {
                yield
                {
                    sock.async_send_to(buffer("message from client"), ep,
                        boost::bind(&server_connection::coro_function, this, boost::placeholders::_1, boost::placeholders::_2));
                }

                yield 
                {
                    sock.async_receive_from(buffer(read_buf, 19), ep,
                        boost::bind(&server_connection::coro_function, this, boost::placeholders::_1, boost::placeholders::_2));
                }

                yield { service.post(boost::bind(&server_connection::on_complete, this, err, bytes)); }
            }
        }
};

int main()
{
    boost::this_thread::sleep(boost::posix_time::millisec(500));

    server_connection sc;
    sc.coro_function();
    service.run();
}