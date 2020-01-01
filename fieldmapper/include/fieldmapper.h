#pragma once
#include <iostream>
#include "include/conversion.h"

namespace ytl::fieldmapper {
    template <typename V, typename R> struct Field {
        using value_type = V;
        using output_type = R;
        using self_type = Field<R, V>;

        const value_type obj;

        const value_type & value() const { return obj; }

        output_type yield() const { return ytl::convert_to<output_type>( value() ); }

        template <typename new_output_type>
        new_output_type yield() const { return ytl::convert_to<new_output_type>( value() ); }
    };

    template <typename V, typename R>
    inline std::ostream & operator << ( std::ostream & os, const Field<R, V> & field ) {
        os << std::to_string( field.getValue() );
        return os;
    }
}

