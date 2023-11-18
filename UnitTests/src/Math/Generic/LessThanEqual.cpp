#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
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
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();

        STATIC_REQUIRE( TRAP::Math::LessThanEqual(max, max));
        STATIC_REQUIRE( TRAP::Math::LessThanEqual(min, min));
        STATIC_REQUIRE(!TRAP::Math::LessThanEqual(max, min));
        STATIC_REQUIRE( TRAP::Math::LessThanEqual(min, max));
        REQUIRE(!TRAP::Math::LessThanEqual(nan, nan));
        STATIC_REQUIRE( TRAP::Math::LessThanEqual(inf, inf));
        STATIC_REQUIRE( TRAP::Math::LessThanEqual(ninf, ninf));
    }
}

TEST_CASE("TRAP::Math::LessThanEqual()", "[math][generic][lessthanequal]")
{
    SECTION("Scalar - i8")
    {
        RunCompileTimeLessThanEqualTests<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunCompileTimeLessThanEqualTests<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunCompileTimeLessThanEqualTests<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunCompileTimeLessThanEqualTests<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunCompileTimeLessThanEqualTests<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunCompileTimeLessThanEqualTests<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunCompileTimeLessThanEqualTests<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunCompileTimeLessThanEqualTests<u64>();
    }
    SECTION("Scalar - f64")
    {
        RunCompileTimeLessThanEqualTests<f64>();
        RunLessThanEqualEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeLessThanEqualTests<f32>();
        RunLessThanEqualEdgeTests<f32>();
    }

    SECTION("Vec2 - i8")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - u8")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - i16")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - u16")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - i32")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - u32")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - i64")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - u64")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - f64")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - i8")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - u8")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - i16")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - u16")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - i32")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - u32")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - i64")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - u64")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - f64")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - i8")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - u8")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - i16")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - u16")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - i32")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - u32")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - i64")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - u64")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - f64")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeLessThanEqualVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - f64")
    {
        RunCompileTimeLessThanEqualQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - f32")
    {
        RunCompileTimeLessThanEqualQuatTests<TRAP::Math::Quatf>();
    }
}