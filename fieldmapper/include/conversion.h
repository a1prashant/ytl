#pragma once
#include <cinttypes>
#include <cstdlib>
#include <type_traits>
#include "CPtrEndingWithSpecialChar.h"

namespace ytl {

    // NOTE: from C++17 onwards to_chars and from_chars will make life easy

    // basic template: value version
    // TODO: Will you need this?

    // basic template: Pointer versions
    template <typename R, typename V> R from_to( const V * v, R & r ) {
        r = *v;
        return r;
    }

    // basic template: reference versions
    template <typename R, typename V> R from_to( const V & v, R & r ) {
        r = v;
        return r;
    }

    template <> std::string from_to( const bool & v, std::string & r ) {
        r = v ? "true" : "false";
        return r;
    }
    template <> std::string from_to( const char & v, std::string & r ) {
        r = std::string( 1, v );
        return r;
    }
    template <> std::string from_to( const unsigned char & v, std::string & r ) {
        r = std::string( 1, v );
        return r;
    }
    template <> std::string from_to( const int16_t & v, std::string & r ) {
        r = std::to_string( v );
        return r;
    }
    template <> std::string from_to( const uint16_t & v, std::string & r ) {
        r = std::to_string( v );
        return r;
    }
    template <> std::string from_to( const int32_t & v, std::string & r ) {
        r = std::to_string( v );
        return r;
    }
    template <> std::string from_to( const uint32_t & v, std::string & r ) {
        r = std::to_string( v );
        return r;
    }
    template <> std::string from_to( const int64_t & v, std::string & r ) {
        r = std::to_string( v );
        return r;
    }
    template <> std::string from_to( const uint64_t & v, std::string & r ) {
        r = std::to_string( v );
        return r;
    }
    template <> std::string from_to( const float & v, std::string & r ) {
        r = std::to_string( v );
        return r;
    }
    template <> std::string from_to( const double & v, std::string & r ) {
        r = std::to_string( v );
        return r;
    }

    template <size_t N> std::string from_to( const std::array<char, N> & v, std::string & r ) {
        r = std::string( v.data(), v.size() );
        return r;
    }

    // specialization, const char * to all ints/floats
    template <> char from_to( const char * v, char & r ) {
        r = std::atoi( v );
        return r;
    }
    template <> unsigned char from_to( const char * v, unsigned char & r ) {
        r = std::atoi( v );
        return r;
    }
    template <> int16_t from_to( const char * v, int16_t & r ) {
        r = std::atol( v );
        return r;
    }
    template <> int32_t from_to( const char * v, int32_t & r ) {
        r = std::atol( v );
        return r;
    }
    template <> uint32_t from_to( const char * v, uint32_t & r ) {
        r = std::atol( v );
        return r;
    }
    template <> int64_t from_to( const char * v, int64_t & r ) {
        r = std::atoll( v );
        return r;
    }
    template <> uint64_t from_to( const char * v, uint64_t & r ) {
        r = std::atoll( v );
        return r;
    }
    template <> float from_to( const char * v, float & r ) {
        r = std::atof( v );
        return r;
    }
    template <> double from_to( const char * v, double & r ) {
        r = std::atof( v );
        return r;
    }

    // specialization, string to all ints/floats
    template <> int16_t from_to( const std::string & v, int16_t & r ) {
        char * end;
        r = std::strtol( v.c_str(), &end, 10 );
        return r;
    }
    template <> uint16_t from_to( const std::string & v, uint16_t & r ) {
        char * end;
        r = std::strtoul( v.c_str(), &end, 10 );
        return r;
    }
    template <> int32_t from_to( const std::string & v, int32_t & r ) {
        char * end;
        r = std::strtol( v.c_str(), &end, 10 );
        return r;
    }
    template <> uint32_t from_to( const std::string & v, uint32_t & r ) {
        char * end;
        r = std::strtoul( v.c_str(), &end, 10 );
        return r;
    }
    template <> int64_t from_to( const std::string & v, int64_t & r ) {
        char * end;
        r = std::strtoll( v.c_str(), &end, 10 );
        return r;
    }
    template <> uint64_t from_to( const std::string & v, uint64_t & r ) {
        char * end;
        r = std::strtoull( v.c_str(), &end, 10 );
        return r;
    }
    template <> float from_to( const std::string & v, float & r ) {
        char * end;
        r = std::strtof( v.c_str(), &end );
        return r;
    }
    template <> double from_to( const std::string & v, double & r ) {
        char * end;
        return std::strtod( v.c_str(), &end );
    }
    template <> long double from_to( const std::string & v, long double & r ) {
        char * end;
        return std::strtold( v.c_str(), &end );
    }

    // specialization: array to all ints/floats, reuses string-version
    template <size_t N> char from_to( const std::array<char, N> & v, char & r ) {
        static_assert( N == 0, "std::array size 0" );
        r = v[ 0 ];
        return r;
    }
    template <size_t N, typename R> R from_to( const std::array<char, N> & v, R & r ) {
        std::string t( v.data(), v.size() );
        return from_to( t, r );
    }

    template <char SPLCHAR, typename V, typename R>
    R from_to( const CPtrEndingWithSpecialChar< V, SPLCHAR > & v, R & r ) {
        return from_to( v.to_string(), r );
    }

    // convert_to is calling made easy for user code
    template <typename R, typename V> R convert_to( const V & v ) {
        R r;
        return from_to( v, r );
    }
}

