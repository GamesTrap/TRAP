#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunCompileTimeMaxScalarTest()
    {
        constexpr T N = static_cast<T>(0);
        constexpr T B = static_cast<T>(1);
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(N, B), B, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(B, N), B, TRAP::Math::Epsilon<T>()));

        constexpr T C = static_cast<T>(2);
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(N, B, C), C, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(B, N, C), C, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(C, N, B), C, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(C, B, N), C, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(B, C, N), C, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(N, C, B), C, TRAP::Math::Epsilon<T>()));

        constexpr T D = static_cast<T>(3);
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(D, N, B, C), D, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(B, D, N, C), D, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(C, N, D, B), D, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(C, B, D, N), D, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(B, C, N, D), D, TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Max(N, C, B, D), D, TRAP::Math::Epsilon<T>()));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunMaxNaNTest()
    {
        static constexpr T NaN = std::numeric_limits<T>::quiet_NaN();
        static constexpr T B = static_cast<T>(1);
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Max(NaN, B)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, NaN)));

        static constexpr T C = static_cast<T>(2);
        REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Max(NaN, B, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, NaN, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, NaN, B)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, B, NaN)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, C, NaN)));
        REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Max(NaN, C, B)));

        static constexpr T D = static_cast<T>(3);
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(D, NaN, B, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, D, NaN, C)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, NaN, D, B)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(C, B, D, NaN)));
        REQUIRE(!TRAP::Math::IsNaN(TRAP::Math::Max(B, C, NaN, D)));
        REQUIRE( TRAP::Math::IsNaN(TRAP::Math::Max(NaN, C, B, D)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T>
    consteval void RunCompileTimeMaxVecTest()
    {
        constexpr T N = static_cast<T>(0);
        constexpr T B = static_cast<T>(1);

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, static_cast<T::value_type>(N.x())), B, TRAP::Math::Epsilon<typename T::value_type>())));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, B), B, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, N), B, TRAP::Math::Epsilon<T>())));

        constexpr T C = static_cast<T>(2);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, B, C), C, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, N, C), C, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, N, B), C, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, B, N), C, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, C, N), C, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, C, B), C, TRAP::Math::Epsilon<T>())));

        constexpr T D = static_cast<T>(3);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(D, N, B, C), D, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, D, N, C), D, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, N, D, B), D, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(C, B, D, N), D, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(B, C, N, D), D, TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Max(N, C, B, D), D, TRAP::Math::Epsilon<T>())));
    }
}

TEST_CASE("TRAP::Math::Max()", "[math][generic][max]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeMaxScalarTest<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeMaxScalarTest<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeMaxScalarTest<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeMaxScalarTest<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeMaxScalarTest<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeMaxScalarTest<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeMaxScalarTest<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeMaxScalarTest<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeMaxScalarTest<double>();
        RunMaxNaNTest<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeMaxScalarTest<float>();
        RunMaxNaNTest<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeMaxVecTest<TRAP::Math::Vec4f>();
    }
}