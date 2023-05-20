#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/signal_set.hpp>

using namespace boost::asio;

io_service service;

void sighandler(const boost::system::error_code& err, int sig)
{
    service.stop();
}

struct client
{
    ip::tcp::socket sock;
    char buf[20];

    client() : sock(service) {}
    ~client() { sock.close(); }
};

class Server
{
    private:
        io_service::work work;
        ip::tcp::acceptor acceptor;
        boost::thread_group threads;

    private:
        void StartThreadPool()
        {
            for (int i = 0; i < 4; ++i)
            {
                threads.create_thread(boost::bind(&io_service::run, &service));
            }
        }

        void on_read(boost::shared_ptr<client> cptr, const boost::system::error_code& err, size_t bytes)
        {
            if (!err)
            {
                std::string msg(cptr->buf, bytes);
                std::cout << "message from client: " << msg << std::endl;
                async_read(cptr->sock, buffer(cptr->buf, 20), boost::bind(&Server::on_read, this, cptr, boost::placeholders::_1, boost::placeholders::_2));
            }
            else
            {
                std::cerr << "on_read: " << err << std::endl;
            }
        }

        void handle_request(boost::shared_ptr<client> cptr)
        {
            async_read(cptr->sock, buffer(cptr->buf, 20), boost::bind(&Server::on_read, this, cptr, boost::placeholders::_1, boost::placeholders::_2));
        }

        void handle_accept(boost::shared_ptr<client> cptr, const boost::system::error_code& err)
        {
            if (!err)
            {
                std::cout << "handle_accept" << std::endl;
                service.post(boost::bind(&Server::handle_request, this, cptr));
                boost::shared_ptr<client> new_cptr(new client);
                acceptor.async_accept(new_cptr->sock, boost::bind(&Server::handle_accept, this, new_cptr, boost::placeholders::_1));
            }
            else
            {
                std::cerr << "handle_accept: " << err << std::endl;
            }
        }

    public:
        Server() : work(service), acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8080)) {}
        void Start()
        {
            std::cout << "Start" << std::endl;
            StartThreadPool();
        }

        void Run()
        {
            std::cout << "Run" << std::endl;
            boost::shared_ptr<client> cptr(new client);
            acceptor.async_accept(cptr->sock, boost::bind(&Server::handle_accept, this, cptr, boost::placeholders::_1));
        }

        void Shutdown()
        {
            threads.join_all();
            acceptor.close();
            std::cout << "Shutdown" << std::endl;
        }
};

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
     * обработка соединений
     */
    Server server;
    server.Start();
    server.Run();
    server.Shutdown();
}