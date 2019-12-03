#include "fieldmapper-test.h"

using namespace ytl::fieldmapper;

// TODO:
// - ytl::convert(a);            // should be preset type
// - ytl::getValue(a);           //
// - ytl::getOutput(a);          //
// - ytl::get<B>(a);             // 
// - ytl::convert<B>(a);         // 
// - auto b = YTL::convert< CField< src_type, NEW_dest_type >( a );


TEST(TestFixture, CheckGetValueFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 'a', field.getValue() );    
}

TEST(TestFixture, CheckOutputFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 97.0f, field.output() );    
}

TEST(TestFixture, CheckGetOutputFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 97.0f, field.getOutput() );    
}

TEST(TestFixture, CheckGetTemplateFn ) {
    char ch = 'a';
    EXPECT_EQ( 97.0f, ytl::get<float>( ch ) );    

    Field< char, float > field{ 'a' };
    EXPECT_EQ( 97.0f, ytl::get( field ) );
}

TEST(TestFixture, CheckConvertTemplateFn) {
    char ch = 'a';
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 97.0f, ytl::convert<float>( ch ) );
}

TEST(TestFixture, CheckConvertTemplateSpecificationFn) {
    char ch = 'a';
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 97.0f, ytl::convert<float>( ch ) );
}

