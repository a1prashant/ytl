#include "gtest/gtest.h"
#include "include/fieldmapper.h"
#include "include/conversion.h"

namespace {

    class TestFixture : public ::testing::Test {
        protected:
            virtual void SetUp() {
            }

            virtual void TearDown() {
            }
    };

    template <typename V, typename R>
    void createFieldAndCheckResult( const V & initialValue, const R & expectedValue ) {
        ytl::fieldmapper::Field< V, R > field{ initialValue };
        EXPECT_EQ( initialValue, field.value() );
        EXPECT_EQ( expectedValue, field.yield() );
    }
}

using namespace ytl::fieldmapper;

TEST(TestFixture, CheckGetValueFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 'a', field.value() );    
}

TEST(TestFixture, CheckOutputFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 97.0f, field.yield() );    
}

TEST(TestFixture, CheckStdConversionFns ) {
    const std::string a = "123.456";

    EXPECT_EQ( 123.456f, ytl::convert_to<float>( a.c_str() ) );    
    EXPECT_EQ( 123l, ytl::convert_to<long>( a.c_str() ) );
    EXPECT_EQ( 123ll, ytl::convert_to<long long>( a.c_str() ) );    
    EXPECT_EQ( 123ul, ytl::convert_to<unsigned long>( a.c_str() ) );    
    EXPECT_EQ( 123ull, ytl::convert_to<unsigned long long>( a.c_str() ) );    
    EXPECT_EQ( 123.456, ytl::convert_to<double>( a.c_str() ) );    

    EXPECT_EQ( 123.456f, ytl::convert_to<float>( a ) );    
    EXPECT_EQ( 123l, ytl::convert_to<long>( a ) );    
    EXPECT_EQ( 123ll, ytl::convert_to<long long>( a ) );    
    EXPECT_EQ( 123u, ytl::convert_to<unsigned long>( a ) );    
    EXPECT_EQ( 123ull, ytl::convert_to<unsigned long long>( a ) );    
    EXPECT_EQ( 123.456, ytl::convert_to<double>( a ) );    

    createFieldAndCheckResult< std::string, float >( "123.456", 123.456f );
    createFieldAndCheckResult< std::string, long >( "123.456", 123l );
    createFieldAndCheckResult< std::string, long long >( "123.456", 123ll );
    createFieldAndCheckResult< std::string, unsigned long >( "123.456", 123ul );
    createFieldAndCheckResult< std::string, unsigned long long >( "123.456", 123ull );
    createFieldAndCheckResult< std::string, double >( "123.456", 123.456 );
}

TEST(TestFixture, CheckGetTemplateFn ) {
    char ch = 'a';
    EXPECT_EQ( 97.0f, ytl::convert_to<float>( ch ) );    
}

