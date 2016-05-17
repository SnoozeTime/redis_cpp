#include "redis_interface.hpp"
#include "debug.hpp"
#include <boost/array.hpp>
using boost::asio::ip::tcp;

namespace rediscpp {

RedisInterface::RedisInterface(std::string host, std::string port):
    socket_(io_)
{
    tcp::resolver resolver(io_);
    tcp::resolver::query query(host, port);

    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
        socket_.close();
        socket_.connect(*endpoint_iterator++, error);
    }
    if (error)
        throw boost::system::system_error(error);

    // Successfully connected yay
}

protocol::RedisReplyPtr  RedisInterface::SendEncodedPacket(std::string packet)
{
    boost::system::error_code error;
    boost::asio::write(socket_, boost::asio::buffer(packet), boost::asio::transfer_all(), error);
    if (error) {
        debug_print("Error while writing to the socket: %s", error.message().c_str());
    }

    // Wait for an answer and read everything
    std::string received_packet;
    auto reply = make_unique<protocol::RedisReply>();
    for (;;)
    {
        boost::array<char, 128> buf;
        boost::system::error_code error;

        size_t len = socket_.read_some(boost::asio::buffer(buf), error);

        if (error == boost::asio::error::eof)
            break; // Connection closed cleanly by peer.
        else if (error)
            throw boost::system::system_error(error); // Some other error.

        received_packet += std::string(buf.data(), len);
        std::cout << received_packet << std::endl;

        // check each step if the reply is a valid redis reply. If not. read more
        // bytes.
        auto partial_reply = protocol::ParseReply(received_packet);
        if (partial_reply->type != protocol::ERROR) {
            reply.swap( partial_reply);
            break;
        }
    }

    // Now, decode and return the redis reply.
    return reply;
}


protocol::RedisReplyPtr RedisInterface::Get(std::string key)
{
    // First, encode the string to send.
    std::vector<std::string> bulk_values = {"GET", key};
    std::string packet;
    // Never fails...
    protocol::EncodeBulkStringArray(bulk_values, packet);

    // Send it and return the reply
    return SendEncodedPacket(packet);
}


}
