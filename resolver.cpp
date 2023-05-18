#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main()
{
    io_service service;
    ip::tcp::resolver resolver(service);
    ip::tcp::resolver::query query("www.google.com", "80");
    ip::tcp::resolver::iterator iter = resolver.resolve(query);
    ip::tcp::endpoint ep = *iter;
    
    std::cout << ep.address().to_string() << std::endl;
    std::cout << ep.port() << std::endl;
}