#include "gtest/gtest.h"
#include "fieldmapper.h"

using namespace ytl::fieldmapper;

TEST(TestFixture, CheckValueFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 'a', field.value() );    
}

TEST(TestFixture, CheckOutputFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 97.0, field.output() );    
}

TEST(TestFixture, CheckGetValueFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 'a', field.getValue() );    
}

TEST(TestFixture, CheckGetOutputFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 97.0, field.getOutput() );    
}

