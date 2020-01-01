#pragma once

namespace ytl {
    // @brief: Ending with spl-char or standard convention of '\0'
    template <typename T, typename SPLCHAR='\0'> struct CPtrEndingWithSpecialChar {
        const char * ptr;
        CPtrEndingWithSpecialChar( const char * p ) : ptr(p) {}
        CPtrEndingWithSpecialChar( const CPtrEndingWithSpecialChar & p ) : ptr( p.ptr ) { }
        const CPtrEndingWithSpecialChar & operator = ( const CPtrEndingWithSpecialChar & p ) {
            ptr = p.ptr;
            return *this;
        }
        inline const T * get() const { return reinterpret_cast< const T * >( ptr ); }
        inline const T & get_cref() const { return * get(); }
        inline const char * c_ptr() const { return ptr; }
        string_view to_string_view() const {
            if( ptr == nullptr ) return {};
            auto mark = ptr;
            while( *mark != '\0' && *mark != SPLCH ) mark++;
            return string_view( ptr, mark - ptr );
        }
        inline std::string to_string() const { return to_string_view().to_string(); }
    };
    template <typename T, typename SPLCHAR>
    inline std::ostream & operator << (std::ostream & os, CPtrEndingWithSpecialChar<T, SPLCHAR> & o ) {
        os << o.to_string();
        return os;
    }
}
