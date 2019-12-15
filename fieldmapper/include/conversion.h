#pragma once
#include <cinttypes>
#include <cstdlib>

namespace ytl {

    // basic templates
    template <typename R, typename V> R convert_to( const V * v ) {
        return *v;
    }
    template <typename R, typename V> R convert_to( const V * v, R & r ) {
        r = convert_to( v );
        return r;
    }
    template <typename R, typename V> R convert_to( const V & v ) {
        return v;
    }
    template <typename R, typename V> R convert_to( const V & v, R & r ) {
        r = convert_to( v );
        return r;
    }

    // specialization
    template <> float convert_to( const char * v ) {
        return std::atof( v );
    }
    template <> long convert_to( const char * v ) {
        return std::atol( v );
    }
    template <> unsigned long convert_to( const char * v ) {
        return std::atol( v );
    }
    template <> long long convert_to( const char * v ) {
        return std::atoll( v );
    }
    template <> unsigned long long convert_to( const char * v ) {
        return std::atoll( v );
    }
    template <> double convert_to( const char * v ) {
        return std::atof( v );
    }
    template <> long convert_to( const std::string & v ) {
        char * end;
        return std::strtol( v.c_str(), &end, 10 );
    }
    template <> long long convert_to( const std::string & v ) {
        char * end;
        return std::strtoll( v.c_str(), &end, 10 );
    }
    template <> unsigned long convert_to( const std::string & v ) {
        char * end;
        return std::strtoul( v.c_str(), &end, 10 );
    }
    template <> unsigned long long convert_to( const std::string & v ) {
        char * end;
        return std::strtoull( v.c_str(), &end, 10 );
    }
    template <> float convert_to( const std::string & v ) {
        char * end;
        return std::strtof( v.c_str(), &end );
    }
    template <> double convert_to( const std::string & v ) {
        char * end;
        return std::strtod( v.c_str(), &end );
    }
    template <> long double convert_to( const std::string & v ) {
        char * end;
        return std::strtold( v.c_str(), &end );
    }
    /* constitutes to the existing fns, so commented
    template <> std::intmax_t convert_to( const std::string & v ) {
        return std::strtoimax( v );
    }
    template <> std::uintmax_t convert_to( const std::string & v ) {
        return std::strtouimax( v );
    }
    */
}

