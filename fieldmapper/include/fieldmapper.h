#pragma once

#include <iostream>
#include "include/converter.h"

namespace ytl::fieldmapper {
    template <typename R, typename V> struct Field {
        using value_type = V;
        using output_type = R;
        using self_type = Field<R, V>;

        const value_type value;

        template<typename... ARGS>
        Field(const ARGS... args ) : value(args...) { }

        const value_type getValue() const { return value; }
        const value_type & getValueRef() const { return value; }

        output_type output() const {
            return ytl::converter::get<output_type>( getValueRef() );
        }
        output_type getOutput() const {
            return ytl::converter::get<output_type>( getValueRef() );
        }

        template <typename T> output_type get() { return converter::get<T>( getValueRef() ); }
        template <typename T> output_type convert() { return converter::get<T>( getValueRef() ); }
    };

    template <typename R, typename V>
    inline std::ostream & operator << ( std::ostream & os, const Field<R, V> & field ) {
        os << std::to_string( field.getValue() );
        return os;
    }

    // TODO:
    // - ytl::convert(a);            // should be preset type
    // - ytl::getValue(a);           //
    // - ytl::getOutput(a);          //
    // - ytl::get<B>(a);             // 
    // - ytl::convert<B>(a);         // 
    // - auto b = YTL::convert< CField< src_type, NEW_dest_type >( a );

}
