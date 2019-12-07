#include "fieldmapper-test.h"

using namespace ytl::fieldmapper;

TEST(TestFixture, CheckGetValueFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 'a', field.value() );    
}

TEST(TestFixture, CheckOutputFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 97.0f, field.yield() );    
}

TEST(TestFixture, CheckGetTemplateFn ) {
    char ch = 'a';
    EXPECT_EQ( 97.0f, ytl::convert_to<float>( ch ) );    
}

