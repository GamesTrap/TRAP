#include <catch2/catch_test_macros.hpp>

#include <array>

#include "Utils/Random/RandomInternal.h"

TEST_CASE("TRAP::Utils::INTERNAL::IsUniformInt", "[utils][random][internal]")
{
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformInt<i8>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformInt<u8>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformInt<char>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformInt<wchar_t>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformInt<char16_t>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformInt<char32_t>);

    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsUniformInt<i16>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsUniformInt<i32>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsUniformInt<i64>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsUniformInt<u16>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsUniformInt<u32>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsUniformInt<u64>);
}

TEST_CASE("TRAP::Utils::INTERNAL::IsUniformReal", "[utils][random][internal]")
{
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsUniformReal<f32>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsUniformReal<f64>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsUniformReal<long double>);

    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<i8>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<i16>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<i32>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<i64>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<u8>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<u16>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<u32>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<u64>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<char>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<wchar_t>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<char16_t>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsUniformReal<char32_t>);
}

TEST_CASE("TRAP::Utils::INTERNAL::IsByte", "[utils][random][internal]")
{
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsByte<i8>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsByte<u8>);

    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<i16>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<i32>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<i64>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<u16>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<u32>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<u64>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<f32>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<f64>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<long double>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<char>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<wchar_t>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<char16_t>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsByte<char32_t>);
}

TEST_CASE("TRAP::Utils::INTERNAL::IsSupportedNumber", "[utils][random][internal]")
{
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<i8>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<u8>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<i16>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<i32>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<i64>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<u16>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<u32>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<u64>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<f32>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<f64>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedNumber<long double>);

    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedNumber<char>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedNumber<wchar_t>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedNumber<char16_t>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedNumber<char32_t>);
}

TEST_CASE("TRAP::Utils::INTERNAL::IsSupportedCharacter", "[utils][random][internal]")
{
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedCharacter<char>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedCharacter<wchar_t>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedCharacter<char16_t>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsSupportedCharacter<char32_t>);

    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<i8>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<u8>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<i16>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<i32>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<i64>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<u16>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<u32>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<u64>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<f32>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<f64>);
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsSupportedCharacter<long double>);
}

TEST_CASE("TRAP::Utils::INTERNAL::IsIterator", "[utils][random][internal]")
{
    class NonIterator
    {};
    STATIC_REQUIRE_FALSE(TRAP::Utils::INTERNAL::IsIterator<NonIterator>);

    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsIterator<std::array<u8, 5u>::iterator>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsIterator<std::array<u8, 5u>::const_iterator>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsIterator<std::array<u8, 5u>::reverse_iterator>);
    STATIC_REQUIRE(TRAP::Utils::INTERNAL::IsIterator<std::array<u8, 5u>::const_reverse_iterator>);
}
