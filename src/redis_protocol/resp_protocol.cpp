#include "redis_protocol/resp_protocol.hpp"
#include "debug.hpp"
#include <iostream>
#include <stdexcept>

namespace rediscpp {
namespace protocol {

EncodeDecodeResult EncodeInteger(const int integer_to_encode, std::string &result)
{
    result = ":" + std::to_string(integer_to_encode) + "\r\n";
    return OK;
}

EncodeDecodeResult DecodeInteger(const std::string integer_to_decode, int& result)
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

EncodeDecodeResult EncodeString(const std::string string_to_encode, std::string& result)
{
    result = "+" + string_to_encode + "\r\n";
    return OK;
}

EncodeDecodeResult DecodeString(const std::string string_to_decode, std::string& result)
{
    // At first, check that size > 2  => +\r\n for empty string
    size_t packet_size = string_to_decode.size();
    if (packet_size < 3) {
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

EncodeDecodeResult EncodeError(const std::string error_to_encode, std::string& result)
{
    result = "-" + error_to_encode + "\r\n";
    return OK;
}

/*
    Will decode an error
*/
EncodeDecodeResult DecodeError(const std::string error_to_decode, std::string& result)
{
    size_t packet_size = error_to_decode.size();
    if (packet_size < 3) {
        return PARSE_ERROR;
    }

    // Then, check that the first character is : and the two last chars are \r\n
    if (error_to_decode[0] != '-' ||
        error_to_decode[packet_size - 2] != '\r' ||
        error_to_decode[packet_size - 1] != '\n') {
        return PARSE_ERROR;
    }

    result = error_to_decode.substr(1, packet_size - 3);
    return OK;
}


EncodeDecodeResult EncodeBulkString(const std::string bulk_string_to_encode, std::string& result)
{
    result =  "$" + std::to_string(bulk_string_to_encode.size()) + "\r\n" + bulk_string_to_encode + "\r\n";
    return OK;
}

EncodeDecodeResult DecodeBulkString(const std::string bulk_string_to_decode, std::string& result)
{
    size_t packet_size = bulk_string_to_decode.size();
    // minimum size is 5: $-1\r\n
    if (packet_size < 5) {
        return PARSE_ERROR;
    }

    // Verify string is a bulk string
    if (bulk_string_to_decode[0] != '$') {
        debug_print("%c is different than $", bulk_string_to_decode[0]);
        return PARSE_ERROR;
    }

    // Check if null string.
    if (bulk_string_to_decode == "$-1\r\n") {
        return NIL;
    }

    // Then, let's get the string size. (first part of the packet.)
    std::string first_part;
    std::string len_str;
    int i;
    for (i = 1; i < packet_size - 2; i++) {
        first_part += bulk_string_to_decode[i];

        if (first_part.size() > 1 &&
            first_part[first_part.size() - 1] == '\n' &&
            first_part[first_part.size() - 2] == '\r') {
            len_str = first_part.substr(0, first_part.size() - 2);
            break;
        }
    }

    // Is it an integer ?
    try {
        int bulk_str_size = std::stoi(len_str);

        // Now, check the size of bulk string is valid. Should be bulk_str_size
        // + 2 (because of last /r/n)
        if (packet_size != i + bulk_str_size + 3) {
            debug_print("Wrong size for bulk string packet: %d\n", i + bulk_str_size + 3);
            return PARSE_ERROR;
        } else if (bulk_string_to_decode[packet_size - 2] != '\r' ||
                   bulk_string_to_decode[packet_size - 1] != '\n') {
            debug_print("%s", "Packet does not finish by \\r\\n");
            return PARSE_ERROR;
        }

        // Should be ok now.
        result = bulk_string_to_decode.substr(i + 1, bulk_str_size + 1);
        return OK;
    } catch (std::invalid_argument &invalid_arg) {
        debug_print("Error in decode_bulk_string: %s - %s \n", invalid_arg.what(), len_str.c_str());
        return PARSE_ERROR;
    } catch (std::out_of_range &oor) {
        debug_print("Out of range in decode_bulk_string: %s\n", oor.what());
        return PARSE_ERROR;
    }

    return OK;
}

EncodeDecodeResult EncodeBulkStringArray(const std::vector<std::string> array_to_encode, std::string& result)
{
    result = "*" + std::to_string(array_to_encode.size()) + "\r\n";
    for (auto& el : array_to_encode) {
        std::string tmp_result;
        EncodeBulkString(el, tmp_result);
        result += tmp_result;
    }
    return OK;
}


RedisReplyPtr ParseReply(const std::string reply_str)
{
    RedisReplyPtr reply = make_unique<RedisReply>();

    // First, check that length is valid.
    if (reply_str.size() < 4) {
        reply->type = ERROR;
        reply->string_value = "The input string is too short to be a redis reply";
    } else {

        // Get the type of the reply.
        char reply_type = reply_str[0];
        if (reply_type == '+') {
            // string
            std::string tmp;
            if (DecodeString(reply_str, tmp) == OK) {
                reply->type = SIMPLE_STRING;
                reply->string_value = tmp;
            } else {
                reply->type = ERROR;
                reply->string_value = "Cannot decode the string value";
            }
        } else if (reply_type == '-') {
            // error string
            // string
            std::string tmp;
            if (DecodeString(reply_str, tmp) == OK) {
                reply->type = SIMPLE_STRING;
                reply->string_value = tmp;
            } else {
                reply->type = ERROR;
                reply->string_value = "Cannot decode the string value";
            }
        } else if (reply_type == ':') {
            /// integer
            // string
            int tmp;
            if (DecodeInteger(reply_str, tmp) == OK) {
                reply->type = INTEGER;
                reply->integer_value = tmp;
            } else {
                reply->type = ERROR;
                reply->string_value = "Cannot decode the integer value";
            }
        } else if (reply_type == '$') {
            // bulk string
            std::string tmp;
            if (DecodeBulkString(reply_str, tmp) == OK) {
                reply->type = BULK_STRING;
                reply->string_value = tmp;
            } else {
                reply->type = ERROR;
                reply->string_value = "Cannot decode the bulk string value";
            }
        } else if (reply_type == '*') {
            // array
            // And here the fun begin.
            if (DecodeArray(reply_str, reply.get()) == OK) {

            } else {
                reply->type = ERROR;
                reply->string_value = "Cannot decode the array.";
            }
        } else {
            //error unknown type.
        }

    }

    return reply;
}

int DecodeArray(const std::string array_to_decode, RedisReply* array)
{
    array->type = ARRAY;

    int index = 1;
    //First, get the size.
    std::string size_str;
    for (index; index < array_to_decode.size() - 1; index++) {

        // First \r\n
        if (array_to_decode[index] == '\r' && array_to_decode[index+1] == '\n') {
            break;
        }

        size_str += array_to_decode[index];
    }

    //Try to convert the size
    int size;
    try {
        size = std::stoi(size_str);
    } catch (std::invalid_argument &invalid_arg) {
        array->type = ERROR;
        array->string_value = "Array error";
        debug_print("Error in decode_arrayr: %s\n", invalid_arg.what());
        return -1;
    } catch (std::out_of_range &oor) {
        array->type = ERROR;
        array->string_value = "Out of range";
        debug_print("Out of range in decode_array: %s\n", oor.what());
        return -1;
    }

    // Now, increment index to point to the begin of the the first element of our
    // array.
    index += 2;

    // Check the size. If 0, return empty array. If -1,return error.
    if (size == 0) {
        return index;
    } else if (size == -1) {
        array->type = ERROR;
        array->string_value = "Array error";
        return -1;
    }

    debug_print("Size of array is %d\n", size);

    while (index < array_to_decode.size() && array->elements.size() != size) {

        if (array_to_decode[index] == ':') {
            // Easy, read until next \r\n
            std::string integer_to_decode;

            for (index; index < array_to_decode.size(); index++) {

                integer_to_decode += array_to_decode[index];
                // First \r\n
                if (array_to_decode[index-1] == '\r' && array_to_decode[index] == '\n') {
                    break;
                }

            }
            debug_print("Integer to decode string: %s\n", integer_to_decode.c_str());

            int integer;
            if (DecodeInteger(integer_to_decode, integer) == OK) {
                RedisReplyPtr integer_element = make_unique<RedisReply>();
                integer_element->type = INTEGER;
                integer_element->integer_value = integer;
                array->AddElementToArray(integer_element);
            } else {
                array->type = ERROR;
                array->string_value = "Cannot decode integer element";
                return -1;
            }

            // Dont forget to update the current reading cursor.
            index += 1;

        } else if (array_to_decode[index] == '+') {
            // Easy, read until next \r\n
            std::string string_to_decode;

            for (index; index < array_to_decode.size(); index++) {
                string_to_decode += array_to_decode[index];
                // First \r\n
                if (array_to_decode[index-1] == '\r' && array_to_decode[index] == '\n') {
                    break;
                }
            }


            std::string string_value;
            if (DecodeString(string_to_decode, string_value) == OK) {
                RedisReplyPtr string_element = make_unique<RedisReply>();
                string_element->type = SIMPLE_STRING;
                string_element->string_value = string_value;
                array->AddElementToArray(string_element);
            } else {
                array->type = ERROR;
                array->string_value = "Cannot decode string element";
                return -1;
            }

            // Dont forget to update the current reading cursor.
            index += 1;

        } else if (array_to_decode[index] == '-') {
            // Easy, read until next \r\n
            // Easy, read until next \r\n
            std::string error_to_decode;

            for (index; index < array_to_decode.size(); index++) {
                error_to_decode += array_to_decode[index];
                // First \r\n
                if (array_to_decode[index-1] == '\r' && array_to_decode[index] == '\n') {
                    break;
                }
            }


            std::string error_value;
            if (DecodeError(error_to_decode, error_value) == OK) {
                RedisReplyPtr error_element = make_unique<RedisReply>();
                error_element->type = SIMPLE_STRING;
                error_element->string_value = error_value;
                array->AddElementToArray(error_element);
            } else {
                array->type = ERROR;
                array->string_value = "Cannot decode error element";
                return -1;
            }

            // Dont forget to update the current reading cursor.
            index += 1;
        } else if (array_to_decode[index] == '$') {
            // Easy, first get size and then it's ok.
            //First, get the size.
            std::string bulk_size_str;
            for (int i = index + 1; i < array_to_decode.size() - 1; i++) {

                // First \r\n
                if (array_to_decode[i] == '\r' && array_to_decode[i+1] == '\n') {
                    break;
                }

                bulk_size_str += array_to_decode[i];
            }
            //Try to convert the size
            int bulk_size;
            try {
                bulk_size = std::stoi(bulk_size_str);
            } catch (std::invalid_argument &invalid_arg) {
                array->type = ERROR;
                array->string_value = "Array error";
                debug_print("Error in decode_arrar: %s\n", invalid_arg.what());
                return -1;
            } catch (std::out_of_range &oor) {
                array->type = ERROR;
                array->string_value = "Out of range";
                debug_print("Out of range in decode_array: %s\n", oor.what());
                return -1;
            }

            // Now, get the substring
            std::string bulk_string_to_decode = array_to_decode.substr(index, 6 + bulk_size);

            // And decode it.
            std::string bulk_string;
            if (DecodeBulkString(bulk_string_to_decode, bulk_string) == OK) {
                RedisReplyPtr bulk_string_element = make_unique<RedisReply>();
                bulk_string_element->type = BULK_STRING;
                bulk_string_element->string_value = bulk_string;
                array->AddElementToArray(bulk_string_element);
            } else {
                array->type = ERROR;
                array->string_value = "Cannot decode bulk string element";
                return -1;
            }

            index += bulk_string_to_decode.size();

        } else if (array_to_decode[index] == '*') {
            //Hell.
            RedisReplyPtr array_element = make_unique<RedisReply>();
            int to_add = DecodeArray(array_to_decode.substr(index), array_element.get());
            array->AddElementToArray(array_element);
            index += to_add;
        } else {
            debug_print("%c is not valid\n", array_to_decode[index]);
            return -1;
        }
    }

    return index;
}



}
}
