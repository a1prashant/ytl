#include <iostream>

namespace ytl::fieldmapper {
    template <typename V, typename O, typename... ARGS> struct Field {
        using value_type = V;
        using output_type = R;
        using self_type = Field<V,O>;

        const value_type value{ARGS&&...};

        Field( const value_type v ) : value(v) {}
        Field( const value_type & v ) : value(v) {}

        const value_type value() const { return value; }
        const value_type getValue() const { return value; }
        const value_type & getValueRef() const { return value; }

        output_type output() const { return changer::get<output_type>( getValueRef() ); }
        output_type getOutput() const { return changer::get<output_type>( getValueRef() ); }

        template <typename T> output_type get() { return changer::get<T>( getValueRef() ); }
        template <typename T> output_type convert() { return changer::get<T>( getValueRef() ); }
    };

    template <typename V, typename O>
    inline std::ostream & operator << ( std::ostream & os, const Field<V,R> & field ) {
        os << std::to_string( field.getValue() );
    }

    // TODO:
    // - ytl::convert(a);            // should be preset type
    // - ytl::getValue(a);           //
    // - ytl::getOutput(a);          //
    // - ytl::get<B>(a);             // 
    // - ytl::convert<B>(a);         // 
    // - auto b = YTL::convert< CField< src_type, NEW_dest_type >( a );

}
