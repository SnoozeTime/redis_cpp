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

rediscpp::protocol::RedisReplyPtr get_reply()
{
    rediscpp::protocol::RedisReplyPtr reply = rediscpp::protocol::RedisReplyPtr(new rediscpp::protocol::RedisReply());
    reply->type = rediscpp::protocol::ARRAY;

    rediscpp::protocol::RedisReplyPtr reply2 = rediscpp::protocol::RedisReplyPtr(new rediscpp::protocol::RedisReply());
    reply2->type = rediscpp::protocol::BULK_STRING;
    reply2->string_value = "hahaha";

    rediscpp::protocol::RedisReplyPtr reply3 = rediscpp::protocol::RedisReplyPtr(new rediscpp::protocol::RedisReply());
    reply3->type = rediscpp::protocol::BULK_STRING;
    reply3->string_value = "hahaha";

    rediscpp::protocol::RedisReplyPtr reply4 = rediscpp::protocol::RedisReplyPtr(new rediscpp::protocol::RedisReply());
    reply4->type = rediscpp::protocol::INTEGER;
    reply4->integer_value = 5;

    reply->AddElementToArray(reply2);
    reply->AddElementToArray(reply3);
    reply->AddElementToArray(reply4);


    return reply;
}

int main()
{
    //rediscpp::protocol::RedisReplyPtr reply = get_reply();
    //rediscpp::protocol::RedisReplyPtr reply = rediscpp::protocol::ParseReply("+OK\r\n");

    //std::cout << reply->string_value << std::endl;
    std::string array_str = "*2\r\n*3\r\n:1\r\n:2\r\n:3\r\n*2\r\n+Foo\r\n-Bar\r\n";

    auto array = make_unique<rediscpp::protocol::RedisReply>();
    DecodeArray(array_str, array.get());
    std::cout << array->string_value << std::endl;
    for (auto& el : array->elements) {
        switch (el->type) {
            case rediscpp::protocol::INTEGER:
                std::cout << "Value is integer: " << el->integer_value << std::endl;
                break;
            case rediscpp::protocol::BULK_STRING:
                std::cout << "Value is bulk string: " << el->string_value << std::endl;
                break;
            case rediscpp::protocol::SIMPLE_STRING:
                std::cout << "Value is bulk string: " << el->string_value << std::endl;
                break;
            case rediscpp::protocol::ERROR:
                std::cout << "Error " << el->string_value << std::endl;
            case rediscpp::protocol::ARRAY:
                std::cout << "Value is array\n";
                for (auto& el2 : el->elements) {
                    switch (el2->type) {
                        case rediscpp::protocol::INTEGER:
                            std::cout << "Value is integer: " << el2->integer_value << std::endl;
                            break;
                        case rediscpp::protocol::BULK_STRING:
                            std::cout << "Value is bulk string: " << el2->string_value << std::endl;
                            break;
                        case rediscpp::protocol::SIMPLE_STRING:
                            std::cout << "Value is bulk string: " << el2->string_value << std::endl;
                            break;
                        case rediscpp::protocol::ERROR:
                            std::cout << "Error " << el2->string_value << std::endl;
                        case rediscpp::protocol::ARRAY:
                            std::cout << "Value is array\n";
                            break;
                        default:
                        break;
                    }
                }
                break;
            default:
            break;
        }
    }


    // reply1.redis_array.push_back(reply2);
    // reply1.redis_array.push_back(reply3);
    // std::string lol = "this is a string";
    // std::string packet;
    //
    // rediscpp::protocol::encode_bulk_string(lol, packet);
    // std::cout << packet << std::endl;
    // std::cout << "---------------------\n";
    //
    // std::string lol2;
    // if (rediscpp::protocol::decode_bulk_string(packet, lol2) == rediscpp::protocol::OK) {
    //     std::cout << lol2 << std::endl;
    // } else {
    //     std::cerr << "Something wrong happened\n";
    // }
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
