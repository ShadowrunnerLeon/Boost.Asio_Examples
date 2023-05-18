#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/signal_set.hpp>

using namespace boost::asio;

io_service service;

/*--------------------------------
 * @brief 
 * helper structs
--------------------------------*/

struct client
{
    ip::udp::endpoint ep;
    char buf[20];
};

/*--------------------------------
 * @brief 
 * helper functions
 --------------------------------*/      

void sighandler(const boost::system::error_code& err, int sig)
{
    service.stop();
}

/*--------------------------------
 * @brief 
 * server class
 --------------------------------*/
 
class Server
{
    private:
        io_service::work work;
        ip::udp::socket server_sock;
        boost::thread_group threads;

        const std::string server_msg = "message from server";

    private:
        void StartThreadPool()
        {
            for (int i = 0; i < 4; ++i)
            {
                threads.create_thread(boost::bind(&io_service::run, &service));
            }
        }  

        void on_recv(boost::shared_ptr<client> cptr, const boost::system::error_code& err, size_t bytes)
        {
            if (!err)
            {
                std::string msg(cptr->buf, bytes);
                std::cout << "message from client: " << msg << std::endl;
            }
            else
            {
                std::cerr << "on_recv: " << err << std::endl;
            }
        }

        void on_send(boost::shared_ptr<client> cptr, const boost::system::error_code& err, size_t bytes)
        {
            if (!err)
            {
                std::cout << "message sent to client: " << bytes << " bytes" << std::endl;
            }
            else
            {
                std::cerr << "on_send: " << err << std::endl;
            }
        }

        void on_async_send(boost::shared_ptr<client> cptr, const boost::system::error_code& err, size_t bytes)
        {
            service.post(boost::bind(&Server::on_send, this, cptr, err, bytes));
        }

        void handle_request(boost::shared_ptr<client> cptr, const boost::system::error_code& err, size_t bytes)
        {
            if (!err)
            {
                service.post(boost::bind(&Server::on_recv, this, cptr, err, bytes));
                server_sock.async_send_to(buffer("Hello from server!", 19), cptr->ep, 
                    boost::bind(&Server::on_async_send, this, cptr, boost::placeholders::_1, boost::placeholders::_2));
            
                boost::shared_ptr<client> new_cptr(new client);
                server_sock.async_receive_from(buffer(new_cptr->buf, 20), new_cptr->ep,
                    boost::bind(&Server::handle_request, this, new_cptr, boost::placeholders::_1, boost::placeholders::_2));
            }
            else
            {
                std::cerr << "handle_request: " << err << std::endl;
            }
        }

    public:
        Server() : work(service), server_sock(service, ip::udp::endpoint(ip::udp::v4(), 8080)) {}
        void Start()
        {
            std::cout << "Start" << std::endl;
            StartThreadPool();
        }

        void Run()
        {
            std::cout << "Run" << std::endl;
            boost::shared_ptr<client> cptr(new client);
            server_sock.async_receive_from(buffer(cptr->buf, 20), cptr->ep,
                boost::bind(&Server::handle_request, this, cptr, boost::placeholders::_1, boost::placeholders::_2));
        }

        void Shutdown()
        {
            threads.join_all();
            server_sock.close();
            std::cout << "Shutdown" << std::endl;
        }
};

int main()
{
    signal_set sigset(service, SIGINT);
    sigset.async_wait(sighandler);

    Server server;
    server.Start();
    server.Run();
    server.Shutdown();
}