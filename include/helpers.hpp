#ifndef REDISCPP_HELPERS_HPP
#define REDISCPP_HELPERS_HPP

#include <string>
/*
    Provides some functions used in the RedisWrapper but which make not sense
    putting in the .hpp because they are not directly related to redis.
*/

// we need to concatenate in a string with spaces.
std::string concat(char /*delimiter*/, std::string str);

template<typename ... Args>
std::string concat(char delimiter, std::string first, Args ... rest)
{
    return first + " " + concat(delimiter, rest...);
}


#endif
