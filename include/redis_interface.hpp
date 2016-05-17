#ifndef REDISINTERFACE_HPP
#define REDISINTERFACE_HPP

#include <boost/asio.hpp>
#include "redis_protocol/resp_protocol.hpp"

/*
    Synchronous connection to a redis server.
    Use boost asio tcp sockets.
*/
namespace rediscpp {
class RedisInterface
{
public:
    /*
        Will create a instance and try to connect to the redis server using
        TCP.

        This will throw an exception if we cannot connect to the server.
        There is not use of this object if it's not connected..
    */
    RedisInterface(std::string host, std::string port);

    /*
        Cannot copy.
    */
    RedisInterface(const RedisInterface&) = delete;
    RedisInterface& operator=(const RedisInterface&) = delete;


    /*
        REDIS API. Most used commands are implement. A general way to write commands
        is also given.
    */

    /*
        Set value for the given key.
        Returns a RedisReplyPtr
    */
    protocol::RedisReplyPtr Get(std::string key);

private:

    /*
        Send a packet and wait for an answer. The packet sent is already encoded
        so we do not make this function available to the user.
    */
    protocol::RedisReplyPtr SendEncodedPacket(std::string packet);

    /*
        This io service object for the boost asio objects
    */
    boost::asio::io_service io_;

    /*
        The connection TCP socket.
    */
    boost::asio::ip::tcp::socket socket_;
};
}

#endif
