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

    // TODO: using PtrField = Field< cptr_splch<V, TOKEN>, R >;
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

TEST(TestFixture, charPtrToStringToBasicPrimitiveDataTypesUsingStdFunctions ) {
    const char * pch = "123.456";
    EXPECT_EQ( 123.456f, ytl::convert_to<float>( pch ) );    
    EXPECT_EQ( 123l, ytl::convert_to<int32_t>( pch ) );
    EXPECT_EQ( 123ll, ytl::convert_to<int64_t>( pch ) );    
    EXPECT_EQ( 123ul, ytl::convert_to<uint32_t>( pch ) );    
    EXPECT_EQ( 123ull, ytl::convert_to<uint64_t>( pch ) );
    EXPECT_EQ( 123.456, ytl::convert_to<double>( pch ) );
}

TEST(TestFixture, stringToBasicPrimitiveDataTypesUsingStdFunctions ) {
    const std::string a = "123.456";

    EXPECT_EQ( 123.456f, ytl::convert_to<float>( a ) );
    EXPECT_EQ( 123l, ytl::convert_to<long>( a ) );    
    EXPECT_EQ( 123ll, ytl::convert_to<int64_t>( a ) );    
    EXPECT_EQ( 123u, ytl::convert_to<uint32_t>( a ) );    
    EXPECT_EQ( 123ull, ytl::convert_to<uint64_t>( a ) );    
    EXPECT_EQ( 123.456, ytl::convert_to<double>( a ) );    
}

TEST(TestFixture, rvalueStringToPrimitiveDataTypesUsingStdFunctions ) {
    createFieldAndCheckResult< std::string, float >( "123.456", 123.456f );
    createFieldAndCheckResult< std::string, int32_t >( "123.456", 123l );
    createFieldAndCheckResult< std::string, int64_t >( "123.456", 123ll );
    createFieldAndCheckResult< std::string, uint32_t >( "123.456", 123ul );
    createFieldAndCheckResult< std::string, uint64_t >( "123.456", 123ull );
    createFieldAndCheckResult< std::string, double >( "123.456", 123.456 );
}

TEST(TestFixture, CheckGetTemplateFn ) {
    char ch = 'a';
    EXPECT_EQ( 97.0f, ytl::convert_to<float>( ch ) );    
}

TEST(TestFixture, CheckToString ) {
    bool               b    = true;
    char               ch   = 'c';
    unsigned char      uch  = 'C';
    short              sh   = 10;
    unsigned short     ush  = 12;
    int                i    = 12;
    unsigned int       ui   = 12;
    long               l    = 12;
    unsigned long      ul   = 12;
    long long          ll   = 12;
    unsigned long long ull  = 12;
    float              f    = 12;
    double             d    = 12;
    long double        ld   = 12;
    EXPECT_EQ( "true", ytl::convert_to<std::string>( b   ) );
    EXPECT_EQ( "c"   , ytl::convert_to<std::string>( ch  ) );
    EXPECT_EQ( "C"   , ytl::convert_to<std::string>( uch ) );
    EXPECT_EQ( "10"  , ytl::convert_to<std::string>( sh  ) );
    EXPECT_EQ( "12"  , ytl::convert_to<std::string>( ush ) );
    EXPECT_EQ( "12"  , ytl::convert_to<std::string>( i   ) );
    EXPECT_EQ( "12"  , ytl::convert_to<std::string>( ui  ) );
    EXPECT_EQ( "12"  , ytl::convert_to<std::string>( l   ) );
    EXPECT_EQ( "12"  , ytl::convert_to<std::string>( ul  ) );
    EXPECT_EQ( "12"  , ytl::convert_to<std::string>( ll  ) );
    EXPECT_EQ( "12"  , ytl::convert_to<std::string>( ull ) );
    EXPECT_EQ( "12.000000"  , ytl::convert_to<std::string>( f   ) );
    EXPECT_EQ( "12.000000"  , ytl::convert_to<std::string>( d   ) );
    EXPECT_EQ( "12.000000"  , ytl::convert_to<std::string>( ld  ) );
}

TEST(TestFixture, CheckArrays ) {
    struct Data {
        char hh[2];
        std::array<char, 2> mm;
    };
    Data data{ { '1', '2' }, { '3', '4' } };
    // EXPECT_EQ( 12ul, ytl::convert_to<unsigned long>( data.hh ) );    
    // EXPECT_EQ( 34l, ytl::convert_to<long>( data.mm ) );    
}

TEST(TestFixture, VariousFieldsToString ) {
    struct Data {
        std::array<char, 2> mm;
        char ch;
        uint8_t ui8;
        uint16_t ui16;
        uint32_t ui32;
        uint64_t ui64;
        int32_t  i32;
        bool flag;
    };

    struct FieldInData {
        Field< std::array<char, 2>, std::string > arr;
        Field< char     , std::string > ch;
        Field< uint8_t  , std::string > ui8;
        Field< uint16_t , std::string > ui16;
        Field< uint32_t , std::string > ui32;
        Field< uint64_t , std::string > ui64;
        Field< int32_t  , std::string > i32;
        Field< bool     , std::string > flag;
    };
    FieldInData data {
        { 'a', 'b' },
            'c',
            8,
            16,
            32,
            64,
            32,
            false
    };
    EXPECT_EQ( "ab", data.arr.yield() );    
    EXPECT_EQ( "c", data.ch.yield() );    
    // EXPECT_EQ( "8", data.ui8.yield() );    
    EXPECT_EQ( "16", data.ui16.yield() );    
    EXPECT_EQ( "32", data.ui32.yield() );    
    EXPECT_EQ( "64", data.ui64.yield() );    
    EXPECT_EQ( "32", data.i32.yield() );    
    EXPECT_EQ( "false", data.flag.yield() );    
}


