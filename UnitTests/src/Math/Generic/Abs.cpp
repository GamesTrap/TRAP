#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::is_arithmetic_v<T>
void RunAbsTests()
{
    const std::array<T, 4> values
    {
        T(0.0), T(-0.0), T(1.0), T(-1.0)
    };

    constexpr T Epsilon = std::numeric_limits<T>::epsilon();
    for(const T val : values)
    {
        if constexpr(std::signed_integral<T> || std::floating_point<T>)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Abs(val), static_cast<T>(std::abs(val)), Epsilon));
        }
        else
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Abs(val), val, Epsilon));
        }
    }
}

template<typename T>
requires std::is_arithmetic_v<T>
constexpr void RunConstexprAbsTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(0.0)), static_cast<T>(0.0), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(-0.0)), static_cast<T>(0.0), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(1.0)), static_cast<T>(1.0), Epsilon));
    if constexpr(std::signed_integral<T> || std::floating_point<T>)
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(-1.0)), static_cast<T>(1.0), Epsilon));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T>
constexpr void RunConstexprAbsVecTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(0.0))), T(static_cast<T::valueType>(0.0)), Epsilon)));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(-0.0))), T(static_cast<T::valueType>(0.0)), Epsilon)));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(1.0))), T(static_cast<T::valueType>(1.0)), Epsilon)));
    if constexpr((std::signed_integral<typename T::valueType> || std::floating_point<typename T::valueType>))
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(-1.0))), T(static_cast<T::valueType>(1.0)), Epsilon)));
    }
}

TEST_CASE("TRAP::Math::Abs()", "[math][generic][abs]")
{
    SECTION("Edge cases")
    {
        constexpr double Epsilon = std::numeric_limits<double>::epsilon();
        constexpr int64_t imin = std::numeric_limits<int64_t>::min();
        constexpr int64_t imax = std::numeric_limits<int64_t>::max();
        constexpr double ninf = -std::numeric_limits<double>::infinity();
        constexpr double inf = std::numeric_limits<double>::infinity();
        constexpr double nan = std::numeric_limits<double>::quiet_NaN();

        REQUIRE(TRAP::Math::Abs(imin) == std::abs(imin));
        REQUIRE(TRAP::Math::Abs(imax) == std::abs(imax));
        REQUIRE((TRAP::Math::IsNaN(TRAP::Math::Abs(nan)) && std::isnan(std::abs(nan))));
        REQUIRE_THAT(TRAP::Math::Abs(ninf), Catch::Matchers::WithinRel(std::abs(ninf), Epsilon));
        REQUIRE_THAT(TRAP::Math::Abs(inf), Catch::Matchers::WithinRel(std::abs(inf), Epsilon));
    }

    SECTION("Scalar - int8_t")
    {
        RunAbsTests<int8_t>();
        RunConstexprAbsTests<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunAbsTests<uint8_t>();
        RunConstexprAbsTests<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunAbsTests<int16_t>();
        RunConstexprAbsTests<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunAbsTests<uint16_t>();
        RunConstexprAbsTests<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunAbsTests<int32_t>();
        RunConstexprAbsTests<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunAbsTests<uint32_t>();
        RunConstexprAbsTests<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunAbsTests<int64_t>();
        RunConstexprAbsTests<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunAbsTests<uint64_t>();
        RunConstexprAbsTests<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunAbsTests<double>();
        RunConstexprAbsTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunAbsTests<float>();
        RunConstexprAbsTests<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4f>();
    }
}