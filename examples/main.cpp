#include <iostream>
#include "redis_protocol/resp_protocol.hpp"
#include "redis_interface.hpp"

void PrintReply(rediscpp::protocol::RedisReply* reply)
{
    switch (reply->type) {
    case rediscpp::protocol::STRING:
        std::cout << "STRING: " << reply->string_value << std::endl;
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

    try {
        rediscpp::RedisInterface redis("localhost", "6379");

        for (int i = 0; i < 100000; i++)
        {
            std::string key = "key" + std::to_string(i);
            auto reply = redis.Get(key);
            PrintReply(reply.get());
        }
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;

}
