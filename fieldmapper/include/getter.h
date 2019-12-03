#pragma once
#include "fieldmapper.h"

namespace ytl {
    template <typename R, typename V> R get( const ytl::fieldmapper::Field<R,V> & v ) {
        return v.getOutput();
    }
    template <typename R, typename V> R get( const V & v ) {
        const ytl::fieldmapper::Field<V, R> field{ v };
        return field.getOutput();
    }
}
