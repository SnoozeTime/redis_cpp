#ifndef REDIS_PROTOCOL_HPP
#define REDIS_PROTOCOL_HPP

#include <string>
/*
    Contains functions to encode and decode data coming to and coming from the
    redis server.

    see http://redis.io/topics/protocol
*/
namespace rediscpp {

    namespace protocol {

        void hello();
        /*
            Return value from the encode / decode value
        */
        enum EncodeDecodeResult {
            OK,
            PARSE_ERROR, // When string do not correspond to integer, ...
        };

        /*
            Will encode an integer;
            This is a CRLF terminated string prefixed by :

            The following commands will reply with an integer reply: SETNX, DEL,
            EXISTS, INCR, INCRBY, DECR, DECRBY, DBSIZE, LASTSAVE, RENAMENX, MOVE,
            LLEN, SADD, SREM, SISMEMBER, SCARD.
        */
        EncodeDecodeResult encode_integer(const int integer_to_encode, std::string &result);

        /*
            Will decode an integer.
        */
        EncodeDecodeResult decode_integer(const std::string integer_to_decode, int& result);

        /*
            Will encode a string. Simple strings are CRLF terminated strings prefixed
            by +
            Simple Strings are used to transmit non binary safe strings with minimal overhead.
        */
        EncodeDecodeResult encode_string(const std::string string_to_encode, std::string& result);

        /*
            Will decode a string
        */
        EncodeDecodeResult decode_string(const std::string string_to_decode, std::string& result);

        /*
            Will encode a bulk string
            Bulk Strings are encoded in the following way:
            A "$" byte followed by the number of bytes composing the string (a prefixed length), terminated by CRLF.
            The actual string data.
            A final CRLF.
        */
        EncodeDecodeResult encode_bulk_string(const std::string bulk_string_to_encode, std::string& result);

        /*
            Will decode a bulk string
        */
        EncodeDecodeResult decode_bulk_string(const std::string bulk_string_to_decode, std::string& result);

    }
}

#endif
