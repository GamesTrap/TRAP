#include <catch2/catch_test_macros.hpp>

#include "Utils/Memory.h"

TEST_CASE("TRAP::Utils::Memory::SwapBytes(integral)", "[utils][memory][swapbytes]")
{
    SECTION("bool")
    {
        bool b = true;
        REQUIRE(b == true);
        TRAP::Utils::Memory::SwapBytes(b);
        REQUIRE(b == true);
        TRAP::Utils::Memory::SwapBytes(b);
        REQUIRE(b == true);
    }

    SECTION("signed char")
    {
        signed char c = 'A';
        REQUIRE(c == 'A');
        TRAP::Utils::Memory::SwapBytes(c);
        REQUIRE(c == 'A');
        TRAP::Utils::Memory::SwapBytes(c);
        REQUIRE(c == 'A');
    }

    SECTION("unsigned char")
    {
        unsigned char c = 'A';
        REQUIRE(c == 'A');
        TRAP::Utils::Memory::SwapBytes(c);
        REQUIRE(c == 'A');
        TRAP::Utils::Memory::SwapBytes(c);
        REQUIRE(c == 'A');
    }

    SECTION("wchar_t")
    {
        wchar_t c = L'€';
        REQUIRE(c == L'€');
        TRAP::Utils::Memory::SwapBytes(c);
        REQUIRE_FALSE(c == L'A');
#ifdef TRAP_PLATFORM_LINUX
        if constexpr (sizeof(wchar_t) == 4)
        {
            REQUIRE(c == L'\xac200000');
        }
        else if constexpr (sizeof(wchar_t) == 2)
        {
            REQUIRE(c == L'\xac20');
        }
#elif defined(TRAP_PLATFORM_WINDOWS)
        REQUIRE(c == L'\xac20');
#endif
        TRAP::Utils::Memory::SwapBytes(c);
        REQUIRE(c == L'€');
    }

    SECTION("i8")
    {
        i8 i = -100;
        REQUIRE(i == -100);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == -100);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == -100);
    }

    SECTION("i16")
    {
        i16 i = -0xABC;
        REQUIRE(i == -0xABC);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 17653);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == -0xABC);
    }

    SECTION("i32")
    {
        i32 i = -0xDEADDEA;
        REQUIRE(i == -0xDEADDEA);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 371332594);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == -0xDEADDEA);
    }

    SECTION("i64")
    {
        i64 i = -0xDEADBEEFDEAD;
        REQUIRE(i == -0xDEADBEEFDEAD);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 5990086852116348927);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == -0xDEADBEEFDEAD);
    }

    SECTION("u8")
    {
        u8 i = 200;
        REQUIRE(i == 200);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 200);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 200);
    }

    SECTION("u16")
    {
        u16 i = 0xABCD;
        REQUIRE(i == 0xABCD);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 0xCDAB);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 0xABCD);
    }

    SECTION("u32")
    {
        u32 i = 0xDEADBEE;
        REQUIRE(i == 0xDEADBEE);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 0xEEDBEA0D);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 0xDEADBEE);
    }

    SECTION("u64")
    {
        u64 i = 0xDEADC0DEC0FE;
        REQUIRE(i == 0xDEADC0DEC0FE);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 0xFEC0DEC0ADDE0000);
        TRAP::Utils::Memory::SwapBytes(i);
        REQUIRE(i == 0xDEADC0DEC0FE);
    }
}

TEST_CASE("TRAP::Utils::Memory::SwapBytes(iter, iter)", "[utils][memory][swapbytes]")
{
    SECTION("bool")
    {
        std::array<bool, 2u> b{true, false};
        REQUIRE(b == std::array<bool, 2u>{true, false});
        TRAP::Utils::Memory::SwapBytes(b.begin(), b.end());
        REQUIRE(b == std::array<bool, 2u>{true, false});
        TRAP::Utils::Memory::SwapBytes(b.begin(), b.end());
        REQUIRE(b == std::array<bool, 2u>{true, false});
    }

    SECTION("signed char")
    {
        std::array<signed char, 2u> c{'A', 'B'};
        REQUIRE(c == std::array<signed char, 2u>{'A', 'B'});
        TRAP::Utils::Memory::SwapBytes(c.begin(), c.end());
        REQUIRE(c == std::array<signed char, 2u>{'A', 'B'});
        TRAP::Utils::Memory::SwapBytes(c.begin(), c.end());
        REQUIRE(c == std::array<signed char, 2u>{'A', 'B'});
    }

    SECTION("unsigned char")
    {
        std::array<unsigned char, 2u> c{'A', 'B'};
        REQUIRE(c == std::array<unsigned char, 2u>{'A', 'B'});
        TRAP::Utils::Memory::SwapBytes(c.begin(), c.end());
        REQUIRE(c == std::array<unsigned char, 2u>{'A', 'B'});
        TRAP::Utils::Memory::SwapBytes(c.begin(), c.end());
        REQUIRE(c == std::array<unsigned char, 2u>{'A', 'B'});
    }

    SECTION("wchar_t")
    {
        std::array<wchar_t, 2u> c{L'€', L'™'};
        REQUIRE(c == std::array<wchar_t, 2u>{L'€', L'™'});
        TRAP::Utils::Memory::SwapBytes(c.begin(), c.end());
#ifdef TRAP_PLATFORM_LINUX
        if constexpr (sizeof(wchar_t) == 4)
        {
            REQUIRE(c == std::array<wchar_t, 2u>{L'\xac200000', L'\x22210000'});
        }
        else if constexpr (sizeof(wchar_t) == 2)
        {
            REQUIRE(c == std::array<wchar_t, 2u>{L'\xac20', L'\x2221'});
        }
#elif defined(TRAP_PLATFORM_WINDOWS)
        REQUIRE(c == std::array<wchar_t, 2u>{L'\xac20', L'\x2221'});
#endif
        TRAP::Utils::Memory::SwapBytes(c.begin(), c.end());
        REQUIRE(c == std::array<wchar_t, 2u>{L'€', L'™'});
    }

    SECTION("i8")
    {
        std::array<i8, 2u> i{-100, 100};
        REQUIRE(i == std::array<i8, 2u>{-100, 100});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<i8, 2u>{-100, 100});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<i8, 2u>{-100, 100});
    }

    SECTION("i16")
    {
        std::array<i16, 2u> i{-0xABC, 0xABC};
        REQUIRE(i == std::array<i16, 2u>{-0xABC, 0xABC});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<i16, 2u>{17653, -17398});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<i16, 2u>{-0xABC, 0xABC});
    }

    SECTION("i32")
    {
        std::array<i32, 2u> i{-0xDEADDEA, 0xDEADDEA};
        REQUIRE(i == std::array<i32, 2u>{-0xDEADDEA, 0xDEADDEA});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<i32, 2u>{371332594, -354555379});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<i32, 2u>{-0xDEADDEA, 0xDEADDEA});
    }

    SECTION("i64")
    {
        std::array<i64, 2u> i{-0xDEADBEEFDEAD, 0xDEADBEEFDEAD};
        REQUIRE(i == std::array<i64, 2u>{-0xDEADBEEFDEAD, 0xDEADBEEFDEAD});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<i64, 2u>{5990086852116348927, -5918029258078420992});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<i64, 2u>{-0xDEADBEEFDEAD, 0xDEADBEEFDEAD});
    }

    SECTION("u8")
    {
        std::array<u8, 2U> i{200, 210};
        REQUIRE(i == std::array<u8, 2U>{200, 210});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<u8, 2U>{200, 210});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<u8, 2U>{200, 210});
    }

    SECTION("u16")
    {
        std::array<u16, 2u> i{0xABCD, 0xEFAB};
        REQUIRE(i == std::array<u16, 2u>{0xABCD, 0xEFAB});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<u16, 2u>{0xCDAB, 0xABEF});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<u16, 2u>{0xABCD, 0xEFAB});
    }

    SECTION("u32")
    {
        std::array<u32, 2u> i{0xDEADBEE, 0xBEDEAD};
        REQUIRE(i == std::array<u32, 2u>{0xDEADBEE, 0xBEDEAD});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<u32, 2u>{0xEEDBEA0D, 0xADDEBE00});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<u32, 2u>{0xDEADBEE, 0xBEDEAD});
    }

    SECTION("u64")
    {
        std::array<u64, 2u> i{0xDEADC0DEC0FE, 0xDEADC0FEC0DE};
        REQUIRE(i == std::array<u64, 2u>{0xDEADC0DEC0FE, 0xDEADC0FEC0DE});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<u64, 2u>{0xFEC0DEC0ADDE0000, 0xDEC0FEC0ADDE0000});
        TRAP::Utils::Memory::SwapBytes(i.begin(), i.end());
        REQUIRE(i == std::array<u64, 2u>{0xDEADC0DEC0FE, 0xDEADC0FEC0DE});
    }
}

TEST_CASE("TRAP::Utils::Memory::ConvertByte(span)", "[utils][memory][convertbyte]")
{
    SECTION("u16")
    {
        static constexpr std::array<u8, 4u> test{0xEF, 0xBE, 0xAD, 0xDE};
        STATIC_REQUIRE(TRAP::Utils::Memory::ConvertByte<u16>(std::span{test}).Value() == 0xBEEF);
    }

    SECTION("u32")
    {
        static constexpr std::array<u8, 4u> test{0xEF, 0xBE, 0xAD, 0xDE};
        STATIC_REQUIRE(TRAP::Utils::Memory::ConvertByte<u32>(std::span{test}).Value() == 0xDEADBEEF);
    }

    SECTION("u64")
    {
        static constexpr std::array<u8, 8u> test{0xEF, 0xBE, 0xAD, 0xDE, 0xDE, 0xC0, 0xAD, 0xDE};
        STATIC_REQUIRE(TRAP::Utils::Memory::ConvertByte<u64>(std::span{test}).Value() == 0xDEADC0DEDEADBEEF);
    }

    SECTION("Invalid")
    {
        STATIC_REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u16>(std::span<const u8>{}));
        STATIC_REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u32>(std::span<const u8>{}));
        STATIC_REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u64>(std::span<const u8>{}));
    }
}
TEST_CASE("TRAP::Utils::Memory::ConvertByte(u8*, len)", "[utils][memory][convertbyte]")
{
    SECTION("u16")
    {
        static constexpr std::array<u8, 4u> test{0xEF, 0xBE, 0xAD, 0xDE};
        REQUIRE(TRAP::Utils::Memory::ConvertByte<u16>(test.data(), test.size()).Value() == 0xBEEF);
    }

    SECTION("u32")
    {
        static constexpr std::array<u8, 4u> test{0xEF, 0xBE, 0xAD, 0xDE};
        REQUIRE(TRAP::Utils::Memory::ConvertByte<u32>(test.data(), test.size()).Value() == 0xDEADBEEF);
    }

    SECTION("u64")
    {
        static constexpr std::array<u8, 8u> test{0xEF, 0xBE, 0xAD, 0xDE, 0xDE, 0xC0, 0xAD, 0xDE};
        REQUIRE(TRAP::Utils::Memory::ConvertByte<u64>(test.data(), test.size()).Value() == 0xDEADC0DEDEADBEEF);
    }

    SECTION("Invalid")
    {
        static constexpr std::array<u8, 4u> test{0xEF, 0xBE, 0xAD, 0xDE};

        STATIC_REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u16>(nullptr, 10u));
        STATIC_REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u32>(nullptr, 10u));
        STATIC_REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u64>(nullptr, 10u));

        REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u16>(test.data(), 0u));
        REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u32>(test.data(), 0u));
        REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u64>(test.data(), 0u));

        STATIC_REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u16>(nullptr, 0u));
        STATIC_REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u32>(nullptr, 0u));
        STATIC_REQUIRE_FALSE(TRAP::Utils::Memory::ConvertByte<u64>(nullptr, 0u));
    }
}

TEST_CASE("TRAP::Utils::Memory::ConvertBytes", "[utils][memory][convertbytes]")
{
    SECTION("u16")
    {
        static constexpr std::array<u8, 8u> test{0xEF, 0xBE, 0xAD, 0xDE, 0xDE, 0xC0, 0xAD, 0xDE};
        std::vector<u16> result{};

        result.resize(test.size() / sizeof(decltype(result)::value_type));
        TRAP::Utils::Memory::ConvertBytes(test.begin(), test.end(), result.begin());
        REQUIRE(result == std::vector<u16>{0xBEEF, 0xDEAD, 0xC0DE, 0xDEAD});

        TRAP::Utils::Memory::ConvertBytes(test.begin(), test.end(), std::back_inserter(result));
        REQUIRE(result == std::vector<u16>{0xBEEF, 0xDEAD, 0xC0DE, 0xDEAD, 0xBEEF, 0xDEAD, 0xC0DE, 0xDEAD});
    }

    SECTION("u32")
    {
        static constexpr std::array<u8, 8u> test{0xEF, 0xBE, 0xAD, 0xDE, 0xDE, 0xC0, 0xAD, 0xDE};
        std::vector<u32> result{};

        result.resize(test.size() / sizeof(decltype(result)::value_type));
        TRAP::Utils::Memory::ConvertBytes(test.begin(), test.end(), result.begin());
        REQUIRE(result == std::vector<u32>{0xDEADBEEF, 0xDEADC0DE});

        TRAP::Utils::Memory::ConvertBytes(test.begin(), test.end(), std::back_inserter(result));
        REQUIRE(result == std::vector<u32>{0xDEADBEEF, 0xDEADC0DE, 0xDEADBEEF, 0xDEADC0DE});
    }

    SECTION("u64")
    {
        static constexpr std::array<u8, 8u> test{0xEF, 0xBE, 0xAD, 0xDE, 0xDE, 0xC0, 0xAD, 0xDE};
        std::vector<u64> result{};

        result.resize(test.size() / sizeof(decltype(result)::value_type));
        TRAP::Utils::Memory::ConvertBytes(test.begin(), test.end(), result.begin());
        REQUIRE(result == std::vector<u64>{0xDEADC0DEDEADBEEF});

        TRAP::Utils::Memory::ConvertBytes(test.begin(), test.end(), std::back_inserter(result));
        REQUIRE(result == std::vector<u64>{0xDEADC0DEDEADBEEF, 0xDEADC0DEDEADBEEF});
    }

    SECTION("Invalid")
    {
        static constexpr std::array<u8, 8u> test{0xEF, 0xBE, 0xAD, 0xDE, 0xDE, 0xC0, 0xAD, 0xDE};
        std::vector<u64> result{};

        TRAP::Utils::Memory::ConvertBytes(test.begin(), test.begin(), result.begin());
        REQUIRE(result.empty());
        TRAP::Utils::Memory::ConvertBytes(test.end(), test.end(), result.begin());
        REQUIRE(result.empty());

        TRAP::Utils::Memory::ConvertBytes(test.begin(), test.begin(), std::back_inserter(result));
        REQUIRE(result.empty());
        TRAP::Utils::Memory::ConvertBytes(test.end(), test.end(), std::back_inserter(result));
        REQUIRE(result.empty());
    }
}
