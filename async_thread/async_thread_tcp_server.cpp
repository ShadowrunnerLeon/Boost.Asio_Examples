#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/signal_set.hpp>

using namespace boost::asio;

io_service service;
io_service::work work(service);
ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8080));
boost::thread_group threads;

const std::string server_msg("message from server");

struct client
{
    ip::tcp::socket sock;
    char buf[20];

    client() : sock(service) {}
};

void sighandler(const boost::system::error_code& err, int sig)
{
    service.stop();
}

void on_read(boost::shared_ptr<client> cptr, const boost::system::error_code& err, size_t bytes)
{
    if (!err)
    {
        std::string msg(cptr->buf, bytes);
        std::cout << "message from client: " << msg << std::endl;
        async_read(cptr->sock, buffer(cptr->buf, 20), boost::bind(on_read, cptr, boost::placeholders::_1, boost::placeholders::_2));
    }
    else
    {
        std::cerr << "on_read: " << err << std::endl;
    }
}

void handle_request(boost::shared_ptr<client> cptr)
{
    async_read(cptr->sock, buffer(cptr->buf, 20), boost::bind(on_read, cptr, boost::placeholders::_1, boost::placeholders::_2));
}

void handle_accept(boost::shared_ptr<client> cptr, const boost::system::error_code& err)
{
    if (!err)
    {
        std::cout << "handle_accept" << std::endl;
        service.post(boost::bind(handle_request, cptr));
        boost::shared_ptr<client> new_cptr(new client);
        acceptor.async_accept(new_cptr->sock, boost::bind(handle_accept, new_cptr, boost::placeholders::_1));
    }
    else
    {
        std::cerr << "handle_accept: " << err << std::endl;
    }
}

int main()
{
    /**
     * @brief 
     * установка обработчика сигнала
     */
    signal_set sigset(service, SIGINT);
    sigset.async_wait(sighandler);


    /**
     * @brief 
     * создание пула потоков
     */
    for (int i = 0; i < 4; ++i)
    {
        threads.create_thread(boost::bind(&io_service::run, &service));
    }

    /**
     * @brief 
     * обработка соединений
     */
    boost::shared_ptr<client> cptr(new client);
    acceptor.async_accept(cptr->sock, boost::bind(handle_accept, cptr, boost::placeholders::_1));

    threads.join_all();
}