#pragma once

#include <string>
#include <sstream>

namespace ytl {
    // @brief: Ending with spl-char or standard convention of '\0'
    template <typename T, char SPLCHAR='\0'> struct CPtrEndingWithSpecialChar {
        const char * ptr = nullptr;
        CPtrEndingWithSpecialChar() = default;
        CPtrEndingWithSpecialChar( const char * p ) : ptr(p) {}
        CPtrEndingWithSpecialChar( const CPtrEndingWithSpecialChar & p ) : ptr( p.ptr ) { }
        void operator = ( const char * p ) { ptr = p; }
        const CPtrEndingWithSpecialChar & operator = ( const CPtrEndingWithSpecialChar & p ) {
            ptr = p.ptr;
            return *this;
        }
        std::string to_string() const {
            if( ptr == nullptr ) return {};
            auto mark = ptr;
            while( *mark != '\0' && *mark != SPLCHAR ) mark++;
            return std::string( ptr, mark - ptr );
        }
    };
    template <typename T, char SPLCHAR = '\0'>
    inline std::ostream & operator << (std::ostream & os, CPtrEndingWithSpecialChar<T, SPLCHAR> & o ) {
        os << o.to_string();
        return os;
    }

}
