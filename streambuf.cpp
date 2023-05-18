#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main()
{
    streambuf out_buf;
    std::ostream output(&out_buf);
    output << "Hello, world!";

    out_buf.commit(7);
    std::istream input(&out_buf);

    std::string data;
    input >> data;

    out_buf.consume(2);

    // "orld!"
    std::cout << &out_buf << std::endl;
    // "Hello, "
    std::cout << data << std::endl;
}