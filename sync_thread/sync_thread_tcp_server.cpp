#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

using namespace boost::asio;

io_service service;

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
        ip::tcp::acceptor acceptor;
        boost::thread_group threads;
        boost::mutex mtx;

    private:
        void HandleRequest()
        {
            while (true)
            {
                client c;

                {
                    boost::mutex::scoped_lock lock(mtx);
                    acceptor.accept(c.sock);
                }
  
                while (true)
                {
                    try
                    {
                        int bytes_from = c.sock.read_some(buffer(c.buf, 20));
                        std::string msg_from_client(c.buf, bytes_from);
                        std::cout << "message from client: " << msg_from_client << std::endl;
                    }
                    catch (const boost::system::system_error& e)
                    {
                        std::cout << "connection was closed by client" << std::endl;
                        break;
                    }

                    int bytes_to = c.sock.write_some(buffer("Hello from server"));
                    std::cout << "message sent to client: " << bytes_to << " bytes" << std::endl;
                }
            }
        }

    public:
        Server() : acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001)) {}

        void Run()
        {
            for (int i = 0; i < 4; ++i)
            {
                threads.create_thread(boost::bind(&Server::HandleRequest, this));
            }
        }

        void Shutdown()
        {
            threads.join_all();
            std::cout << "Shutdown" << std::endl;
        }
};

int main()
{
    Server server;
    server.Run();
    server.Shutdown();
}