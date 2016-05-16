#include "redis_protocol/resp_protocol.hpp"
#include "debug.hpp"
#include <iostream>
#include <stdexcept>

namespace rediscpp {
namespace protocol {

EncodeDecodeResult encode_integer(const int integer_to_encode, std::string &result)
{
    result = ":" + std::to_string(integer_to_encode) + "\r\n";
    return OK;
}

EncodeDecodeResult decode_integer(const std::string integer_to_decode, int& result)
{
    // At first, check that size > 4 (needs the :, the number and the \r\n)
    size_t packet_size = integer_to_decode.size();
    if (packet_size < 4) {
        return PARSE_ERROR;
    }

    // Then, check that the first character is : and the two last chars are \r\n
    if (integer_to_decode[0] != ':' ||
        integer_to_decode[packet_size - 2] != '\r' ||
        integer_to_decode[packet_size - 1] != '\n') {
        return PARSE_ERROR;
    }

    // Now, we have to convert from string to integer.
    try {
        std::string int_str = integer_to_decode.substr(1, packet_size - 3);
        result = std::stoi(int_str);
        return OK;
    } catch (std::invalid_argument &invalid_arg) {
        debug_print("Error in decode_integer: %s\n", invalid_arg.what());
        return PARSE_ERROR;
    } catch (std::out_of_range &oor) {
        debug_print("Out of range in decode_integer: %s\n", oor.what());
        return PARSE_ERROR;
    }
}

EncodeDecodeResult encode_string(const std::string string_to_encode, std::string& result)
{
    result = "+" + string_to_encode + "\r\n";
    return OK;
}

EncodeDecodeResult decode_string(const std::string string_to_decode, std::string& result)
{
    // At first, check that size > 4 (needs the :, the number and the \r\n)
    size_t packet_size = string_to_decode.size();
    if (packet_size < 4) {
        return PARSE_ERROR;
    }

    // Then, check that the first character is : and the two last chars are \r\n
    if (string_to_decode[0] != '+' ||
        string_to_decode[packet_size - 2] != '\r' ||
        string_to_decode[packet_size - 1] != '\n') {
        return PARSE_ERROR;
    }

    result = string_to_decode.substr(1, packet_size - 3);
    return OK;
}

EncodeDecodeResult encode_bulk_string(const std::string bulk_string_to_encode, std::string& result)
{
    result =  "$" + std::to_string(bulk_string_to_encode.size()) + "\r\n" + bulk_string_to_encode + "\r\n";
    return OK;
}

EncodeDecodeResult decode_bulk_string(const std::string bulk_string_to_decode, std::string& result)
{
    size_t packet_size = bulk_string_to_decode.size();
    // minimum size is 5: "$0\r\n\r\n"
    if (packet_size < 5) {
        return PARSE_ERROR;
    }

    // Verify string is a bulk string

    // Then, let's get the string size. (first part of the packet.)
    std::string first_part;

    return OK;
}

}
}
