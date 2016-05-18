#include <iostream>
#include "redis_protocol/resp_protocol.hpp"
#include "redis_interface.hpp"

void PrintReply(rediscpp::protocol::RedisReply* reply)
{
    switch (reply->type) {
    case rediscpp::protocol::BULK_STRING:
        std::cout << "BULK STRING: " << reply->string_value << std::endl;
        break;
    case rediscpp::protocol::SIMPLE_STRING:
        std::cout << "SIMPLE STRING: " << reply->string_value << std::endl;
        break;
    case rediscpp::protocol::INTEGER:
        std::cout << "INTEGER: " << reply->integer_value << std::endl;
        break;
    case rediscpp::protocol::ERROR:
        std::cout << "ERROR: " << reply->string_value << std::endl;
        break;
    case rediscpp::protocol::NIL_VALUE:
        std::cout << "NIL VALUE\n";
        break;
    case rediscpp::protocol::ARRAY:
        std::cout << "Got an array of " << reply->elements.size() << " elements\n";
        for (auto& el : reply->elements) {
            PrintReply(el.get());
        }
        break;
    }
}

int main()
{


    rediscpp::RedisInterface redis("localhost", "6379");

    {
        auto reply = redis.BrpopTimeout("mylist", 2);
        PrintReply(reply.get());
    }

    return 0;

}
