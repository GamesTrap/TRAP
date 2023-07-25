#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::is_arithmetic_v<T>
consteval void RunCompileTimeLessThanTests()
{
    constexpr T A(T(1.0f));
    constexpr T B(T(0.0f));

    static_assert(!TRAP::Math::LessThan(A, B));
    static_assert( TRAP::Math::LessThan(B, A));

    if constexpr(std::floating_point<T> || std::signed_integral<T>)
    {
        constexpr T C(T(-1.0f));

        static_assert(!TRAP::Math::LessThan(B, C));
        static_assert( TRAP::Math::LessThan(C, B));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T>
consteval void RunCompileTimeLessThanVecTests()
{
    constexpr T A(TRAP::Math::Vec<4, typename T::valueType>(2, 3, 4, 5));
    constexpr T B(TRAP::Math::Vec<4, typename T::valueType>(1, 2, 3, 4));

    static_assert(!TRAP::Math::All(TRAP::Math::LessThan(A, B)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThan(B, A)));

    if constexpr(std::floating_point<typename T::valueType> || std::signed_integral<typename T::valueType>)
    {
        constexpr T C(TRAP::Math::Vec<4, typename T::valueType>(-2, -3, -4, -5));

        static_assert(!TRAP::Math::All(TRAP::Math::LessThan(A, C)));
        static_assert( TRAP::Math::All(TRAP::Math::LessThan(C, A)));
    }
}

template<typename T>
requires TRAP::Math::IsQuat<T>
consteval void RunCompileTimeLessThanQuatTests()
{
    constexpr T A(5.0f, 5.0f, 5.0f, 5.0f);
    constexpr T B(0.0f, 0.0f, 0.0f, 0.0f);
    constexpr T C(-1.0f, -1.0f, -1.0f, -1.0f);

    static_assert(!TRAP::Math::All(TRAP::Math::LessThan(A, B)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThan(B, A)));
    static_assert(!TRAP::Math::All(TRAP::Math::LessThan(A, C)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThan(C, A)));
}

template<typename T>
requires std::floating_point<T>
void RunLessThanEdgeTests()
{
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    static_assert(!TRAP::Math::LessThan(max, max));
    static_assert(!TRAP::Math::LessThan(min, min));
    static_assert(!TRAP::Math::LessThan(max, min));
    static_assert( TRAP::Math::LessThan(min, max));
    REQUIRE(!TRAP::Math::LessThan(nan, nan));
    static_assert(!TRAP::Math::LessThan(inf, inf));
    static_assert(!TRAP::Math::LessThan(ninf, ninf));
}

TEST_CASE("TRAP::Math::LessThan()", "[math][generic][lessthan]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeLessThanTests<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeLessThanTests<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeLessThanTests<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeLessThanTests<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeLessThanTests<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeLessThanTests<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeLessThanTests<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeLessThanTests<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeLessThanTests<double>();
        RunLessThanEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeLessThanTests<float>();
        RunLessThanEdgeTests<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeLessThanVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunCompileTimeLessThanQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunCompileTimeLessThanQuatTests<TRAP::Math::Quatf>();
    }
}