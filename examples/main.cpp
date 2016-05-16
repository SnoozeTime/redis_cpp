#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

/*
    Encode a bulk string.
    '$' + size + '\r\n' + string + '\r\n'
    Should not contain \r or \n
*/
//
// std::string encode_bulk_string(std::string bulk_string);
// std::string encode_bulk_array(std::vector<std::string> array);

#include "redis_protocol/resp_protocol.hpp"

int main()
{
    std::string lol = ":q\r\n";
    int i;
    if (rediscpp::protocol::decode_integer(lol, i) == rediscpp::protocol::OK) {
        std::cout << lol << std::endl;
    } else {
        std::cerr << "Something wrong happened\n";
    }
    // std::vector<std::string> haha;
    // haha.push_back("LPUSH");
    // haha.push_back("mylist");
    // haha.push_back("Lol this is funny");
    // boost::asio::io_service io_service;
    // tcp::resolver resolver(io_service);
    // tcp::resolver::query query("localhost", "6379");
    //
    // tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    // tcp::resolver::iterator end;
    // tcp::socket socket(io_service);
    // boost::system::error_code error = boost::asio::error::host_not_found;
    // while (error && endpoint_iterator != end)
    // {
    //     socket.close();
    //     socket.connect(*endpoint_iterator++, error);
    // }
    // if (error)
    //     throw boost::system::system_error(error);
    //
    // {
    //     std::string message = encode_bulk_array(haha);
    //     boost::system::error_code ignored_error;
    //     boost::asio::write(socket, boost::asio::buffer(message), boost::asio::transfer_all(), ignored_error);
    //
    //     boost::asio::streambuf b;
    //     boost::asio::read_until(socket, b, "\r\n");
    //     std::istream is(&b);
    //     std::string line;
    //     std::getline(is, line);
    //     std::cout << line << std::endl;
    // }

    return 0;

}

//
// std::string encode_bulk_string(std::string bulk_string)
// {
//     return "$" + std::to_string(bulk_string.size()) + "\r\n" + bulk_string + "\r\n";
// }
//
// std::string encode_bulk_array(std::vector<std::string> array)
// {
//     std::string packet_encoded = "*" + std::to_string(array.size()) + "\r\n";
//     for (auto bulk_string : array) {
//         packet_encoded += encode_bulk_string(bulk_string);
//     }
//     return packet_encoded;
// }
