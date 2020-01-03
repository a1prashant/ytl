#include "gtest/gtest.h"
#include "include/fieldmapper.h"
#include "include/conversion.h"
#include "include/CPtrEndingWithSpecialChar.h"

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

    template <typename V, char TOKEN, typename R>
    using PtrSplCharField = ytl::fieldmapper::Field< ytl::CPtrEndingWithSpecialChar<V, TOKEN>, R >;

    using PtrCsv = ytl::CPtrEndingWithSpecialChar< char, ',' >;
}

using namespace ytl::fieldmapper;

TEST(TestFixture, CheckGetValueFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 'a', field.value() );    
}

TEST(TestFixture, CheckYieldFn) {
    Field< char, float > field{ 'a' };
    EXPECT_EQ( 97.0f, field.yield() );    
}

TEST(TestFixture, CheckBasicConvertToFn ) {
    char ch = 'a';
    EXPECT_EQ( 97.0f, ytl::convert_to<float>( ch ) );    
}

TEST(TestFixture, CharPtrOfNumberString_To_BasicTypes ) {
    const char * pch = "123.456";
    EXPECT_EQ( 123.456f, ytl::convert_to<float>( pch ) );    
    EXPECT_EQ( 123l, ytl::convert_to<int32_t>( pch ) );
    EXPECT_EQ( 123ll, ytl::convert_to<int64_t>( pch ) );    
    EXPECT_EQ( 123ul, ytl::convert_to<uint32_t>( pch ) );    
    EXPECT_EQ( 123ull, ytl::convert_to<uint64_t>( pch ) );
    EXPECT_EQ( 123.456, ytl::convert_to<double>( pch ) );
}

TEST(TestFixture, String_To_BasicTypes ) {
    const std::string a = "123.456";

    EXPECT_EQ( 123.456f, ytl::convert_to<float>( a ) );
    EXPECT_EQ( 123l, ytl::convert_to<long>( a ) );    
    EXPECT_EQ( 123ll, ytl::convert_to<int64_t>( a ) );    
    EXPECT_EQ( 123u, ytl::convert_to<uint32_t>( a ) );    
    EXPECT_EQ( 123ull, ytl::convert_to<uint64_t>( a ) );    
    EXPECT_EQ( 123.456, ytl::convert_to<double>( a ) );    
}

TEST(TestFixture, RvalueString_To_BasicTypes ) {
    createFieldAndCheckResult< std::string, float >( "123.456", 123.456f );
    createFieldAndCheckResult< std::string, int32_t >( "123.456", 123l );
    createFieldAndCheckResult< std::string, int64_t >( "123.456", 123ll );
    createFieldAndCheckResult< std::string, uint32_t >( "123.456", 123ul );
    createFieldAndCheckResult< std::string, uint64_t >( "123.456", 123ull );
    createFieldAndCheckResult< std::string, double >( "123.456", 123.456 );
}

TEST(TestFixture, BasicTypes_To_String ) {
    bool                b   = true;
    char                ch  = 'c';
    unsigned char       u8  = 'C';
    int16_t             i16 = 10;
    uint16_t            u16 = 12;
    int32_t             i32 = 12;
    uint32_t            u32 = 12;
    int64_t             i64 = 12;
    uint64_t            u64 = 12;
    float               f   = 12;
    double              d   = 12;
    EXPECT_EQ( "true"       , ytl::convert_to<std::string>( b   ) );
    EXPECT_EQ( "c"          , ytl::convert_to<std::string>( ch  ) );
    EXPECT_EQ( "C"          , ytl::convert_to<std::string>( u8  ) );
    EXPECT_EQ( "10"         , ytl::convert_to<std::string>( i16 ) );
    EXPECT_EQ( "12"         , ytl::convert_to<std::string>( u16 ) );
    EXPECT_EQ( "12"         , ytl::convert_to<std::string>( i32 ) );
    EXPECT_EQ( "12"         , ytl::convert_to<std::string>( u32 ) );
    EXPECT_EQ( "12"         , ytl::convert_to<std::string>( i64 ) );
    EXPECT_EQ( "12"         , ytl::convert_to<std::string>( u64 ) );
    EXPECT_EQ( "12.000000"  , ytl::convert_to<std::string>( f   ) );
    EXPECT_EQ( "12.000000"  , ytl::convert_to<std::string>( d   ) );
}

TEST(TestFixture, CheckArrays ) {
    struct Data {
        char hh[2];
        std::array<char, 2> mm;
    };
    Data data{ { '1', '2' }, { '3', '4' } };
    // EXPECT_EQ( 12ul, ytl::convert_to<uint32_t>( data.hh ) );    
    EXPECT_EQ( 34l, ytl::convert_to<int32_t>( data.mm ) );    
}

TEST(TestFixture, VariousFieldsStructMembers_To_String ) {
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

TEST(TestFixture, CustomDateString_To_Numbers) {
    struct Data {
        std::array<char, 2> dd;
        char seperator_dd;
        std::array<char, 2> mm;
        char seperator_mm;
        std::array<char, 4> yyyy;
    };
    Data data{ { '2', '7' }, '/', { '1', '2' }, '/', { '2', '0', '1', '9' } };
    EXPECT_EQ( 27, ytl::convert_to<uint16_t>( data.dd ) );
    EXPECT_EQ( 12, ytl::convert_to<uint16_t>( data.mm ) );
    EXPECT_EQ( 2019, ytl::convert_to<uint16_t>( data.yyyy ) );
}

TEST( TestFixture, CustomEndSplCh_To_Data ) {
    const char * input = "abc,def";

    PtrCsv csv{ input };
    Field< PtrCsv, std::string > data{ csv };
    EXPECT_EQ( "abc", data.yield() );
}

TEST( TestFixture, CSV_To_Data ) {
    const char * csv = "one,2.2,three,4,five";
    struct Data {
        std::string first;
        std::string second;
        std::string third;
        uint32_t fourth;
        std::string fifth;
    };

    struct FieldInData {
        PtrCsv first;
        PtrCsv second;
        PtrCsv third;
        PtrCsv fourth;
        PtrCsv fifth;
    };
    FieldInData ptrStruct;

    // a small dirty csv parser - start
    {
        auto addToStruct = [&]( const auto * p, auto count ) {
            switch( count ) {
                case 0: ptrStruct.first = p; break;
                case 1: ptrStruct.second = p; break;
                case 2: ptrStruct.third = p; break;
                case 3: ptrStruct.fourth = p; break;
                case 4: ptrStruct.fifth = p; break;
            }
        };
        const char * p = csv;
        int count = 0;
        while( *p != '\0' ) {
            if( count == 0 ) {
                addToStruct( p, count++ );
            } else if( *p == ',' ) {
                addToStruct( ++p, count++ );
            } else {
                p++;
            }
        }
        std::cout 
            << ptrStruct.first  << "..."
            << ptrStruct.second << "..."
            << ptrStruct.third  << "..."
            << ptrStruct.fourth << "..."
            << ptrStruct.fifth  << "..."
            ;
    }
    // a small dirty csv parser - end

    EXPECT_EQ( "one"    , ytl::convert_to< std::string >( ptrStruct.first  ) );
    EXPECT_EQ( 2.2f     , ytl::convert_to< float >( ptrStruct.second ) );
    EXPECT_EQ( "three"  , ytl::convert_to< std::string >( ptrStruct.third  ) );
    EXPECT_EQ( 4ul      , ytl::convert_to< uint32_t    >( ptrStruct.fourth ) );
    EXPECT_EQ( "five"   , ytl::convert_to< std::string >( ptrStruct.fifth  ) );
}


