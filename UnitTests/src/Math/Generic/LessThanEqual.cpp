#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::is_arithmetic_v<T>
consteval void RunCompileTimeLessThanEqualTests()
{
    constexpr T A(T(1.0f));
    constexpr T B(T(0.0f));

    static_assert(!TRAP::Math::LessThanEqual(A, B));
    static_assert( TRAP::Math::LessThanEqual(B, A));
    static_assert( TRAP::Math::LessThanEqual(A, A));
    static_assert( TRAP::Math::LessThanEqual(B, B));

    if constexpr(std::floating_point<T> || std::signed_integral<T>)
    {
        constexpr T C(T(-1.0f));

        static_assert(!TRAP::Math::LessThanEqual(B, C));
        static_assert( TRAP::Math::LessThanEqual(C, B));
        static_assert( TRAP::Math::LessThanEqual(C, C));
    }
}

template<typename T>
requires TRAP::Math::IsVec<T>
consteval void RunCompileTimeLessThanEqualVecTests()
{
    constexpr T A(TRAP::Math::Vec<4, typename T::value_type>(2, 3, 4, 5));
    constexpr T B(TRAP::Math::Vec<4, typename T::value_type>(1, 2, 3, 4));

    static_assert(!TRAP::Math::All(TRAP::Math::LessThanEqual(A, B)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThanEqual(B, A)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThanEqual(A, A)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThanEqual(B, B)));

    if constexpr(std::floating_point<typename T::value_type> || std::signed_integral<typename T::value_type>)
    {
        constexpr T C(TRAP::Math::Vec<4, typename T::value_type>(-2, -3, -4, -5));

        static_assert(!TRAP::Math::All(TRAP::Math::LessThanEqual(A, C)));
        static_assert( TRAP::Math::All(TRAP::Math::LessThanEqual(C, A)));
        static_assert( TRAP::Math::All(TRAP::Math::LessThanEqual(C, C)));
    }
}

template<typename T>
requires TRAP::Math::IsQuat<T>
consteval void RunCompileTimeLessThanEqualQuatTests()
{
    constexpr T A(5.0f, 5.0f, 5.0f, 5.0f);
    constexpr T B(0.0f, 0.0f, 0.0f, 0.0f);
    constexpr T C(-1.0f, -1.0f, -1.0f, -1.0f);

    static_assert(!TRAP::Math::All(TRAP::Math::LessThanEqual(A, B)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThanEqual(B, A)));
    static_assert(!TRAP::Math::All(TRAP::Math::LessThanEqual(A, C)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThanEqual(C, A)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThanEqual(A, A)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThanEqual(B, B)));
    static_assert( TRAP::Math::All(TRAP::Math::LessThanEqual(C, C)));
}

template<typename T>
requires std::floating_point<T>
void RunLessThanEqualEdgeTests()
{
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    static_assert( TRAP::Math::LessThanEqual(max, max));
    static_assert( TRAP::Math::LessThanEqual(min, min));
    static_assert(!TRAP::Math::LessThanEqual(max, min));
    static_assert( TRAP::Math::LessThanEqual(min, max));
    REQUIRE(!TRAP::Math::LessThanEqual(nan, nan));
    static_assert( TRAP::Math::LessThanEqual(inf, inf));
    static_assert( TRAP::Math::LessThanEqual(ninf, ninf));
}

TEST_CASE("TRAP::Math::LessThanEqual()", "[math][generic][lessthanequal]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeLessThanEqualTests<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeLessThanEqualTests<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeLessThanEqualTests<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeLessThanEqualTests<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeLessThanEqualTests<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeLessThanEqualTests<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeLessThanEqualTests<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeLessThanEqualTests<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeLessThanEqualTests<double>();
        RunLessThanEqualEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeLessThanEqualTests<float>();
        RunLessThanEqualEdgeTests<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunCompileTimeLessThanEqualQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunCompileTimeLessThanEqualQuatTests<TRAP::Math::Quatf>();
    }
}