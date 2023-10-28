#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunCompileTimeEqualTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr T A(T(0.0f));
        constexpr T B(T(5.0f));

        static_assert(TRAP::Math::Equal(A, A));
        static_assert(TRAP::Math::Equal(B, B));
        static_assert(!TRAP::Math::Equal(A, B));
        static_assert(!TRAP::Math::Equal(B, A));

        static_assert(TRAP::Math::Equal(A, A, Epsilon));
        static_assert(TRAP::Math::Equal(B, B, Epsilon));
        static_assert(!TRAP::Math::Equal(A, B, Epsilon));
        static_assert(!TRAP::Math::Equal(B, A, Epsilon));

        if constexpr(std::floating_point<T> || std::signed_integral<T>)
        {
            constexpr T C(T(-1.0f));
            constexpr T D(T(-5.0f));

            static_assert(TRAP::Math::Equal(C, C));
            static_assert(TRAP::Math::Equal(D, D));
            static_assert(!TRAP::Math::Equal(C, D));
            static_assert(!TRAP::Math::Equal(D, C));

            static_assert(TRAP::Math::Equal(C, C, Epsilon));
            static_assert(TRAP::Math::Equal(D, D, Epsilon));
            static_assert(!TRAP::Math::Equal(C, D, Epsilon));
            static_assert(!TRAP::Math::Equal(D, C, Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T>
    consteval void RunCompileTimeEqualVecTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        constexpr T A(T(0));
        constexpr T B(T(5));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(A, A)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(B, B)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(A, B)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(B, A)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(A, A, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(B, B, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(A, B, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(B, A, Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(A, A, T(Epsilon))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(B, B, T(Epsilon))));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(A, B, T(Epsilon))));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(B, A, T(Epsilon))));

        if constexpr(std::floating_point<typename T::value_type> || std::signed_integral<typename T::value_type>)
        {
            constexpr T C(T(-1));
            constexpr T D(T(-5));

            static_assert(TRAP::Math::All(TRAP::Math::Equal(C, C)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(D, D)));
            static_assert(!TRAP::Math::All(TRAP::Math::Equal(C, D)));
            static_assert(!TRAP::Math::All(TRAP::Math::Equal(D, C)));

            static_assert(TRAP::Math::All(TRAP::Math::Equal(C, C, Epsilon)));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(D, D, Epsilon)));
            static_assert(!TRAP::Math::All(TRAP::Math::Equal(C, D, Epsilon)));
            static_assert(!TRAP::Math::All(TRAP::Math::Equal(D, C, Epsilon)));

            static_assert(TRAP::Math::All(TRAP::Math::Equal(C, C, T(Epsilon))));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(D, D, T(Epsilon))));
            static_assert(!TRAP::Math::All(TRAP::Math::Equal(C, D, T(Epsilon))));
            static_assert(!TRAP::Math::All(TRAP::Math::Equal(D, C, T(Epsilon))));
        }
    }

    template<typename T>
    requires TRAP::Math::IsMat<T>
    consteval void RunCompileTimeEqualMatTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        constexpr T A(0.0f);
        constexpr T B(5.0f);
        constexpr T C(-1.0f);
        constexpr T D(-5.0f);

        static_assert(TRAP::Math::All(TRAP::Math::Equal(A, A)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(B, B)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(C, C)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(D, D)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(A, B)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(B, A)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(A, A, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(B, B, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(C, C, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(D, D, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(A, B, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(B, A, Epsilon)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(A, A, typename T::col_type(Epsilon))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(B, B, typename T::col_type(Epsilon))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(C, C, typename T::col_type(Epsilon))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(D, D, typename T::col_type(Epsilon))));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(A, B, typename T::col_type(Epsilon))));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(B, A, typename T::col_type(Epsilon))));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    consteval void RunCompileTimeEqualQuatTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        constexpr T A(0.0f, 0.0f, 0.0f, 0.0f);
        constexpr T B(5.0f, 5.0f, 5.0f, 5.0f);
        constexpr T C(-1.0f, -1.0f, -1.0f, -1.0f);
        constexpr T D(-5.0f, -5.0f, -5.0f, -5.0f);

        static_assert(TRAP::Math::All(TRAP::Math::Equal(A, A)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(B, B)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(C, C)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(D, D)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(A, B)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(B, A)));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(A, A, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(B, B, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(C, C, Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(D, D, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(A, B, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::Equal(B, A, Epsilon)));
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunCompileTimeEqualEdgeTests()
    {
        constexpr T max = std::numeric_limits<T>::max();
        constexpr T min = std::numeric_limits<T>::lowest();
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();

        static_assert(TRAP::Math::Equal(max, max));
        static_assert(TRAP::Math::Equal(min, min));
        static_assert(!TRAP::Math::Equal(max, min));
        static_assert(!TRAP::Math::Equal(min, max));
        static_assert(!TRAP::Math::Equal(nan, nan));
        static_assert(TRAP::Math::Equal(inf, inf));
        static_assert(TRAP::Math::Equal(ninf, ninf));
    }
}

TEST_CASE("TRAP::Math::Equal()", "[math][generic][equal]")
{
    SECTION("Scalar - i8")
    {
        RunCompileTimeEqualTests<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunCompileTimeEqualTests<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunCompileTimeEqualTests<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunCompileTimeEqualTests<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunCompileTimeEqualTests<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunCompileTimeEqualTests<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunCompileTimeEqualTests<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunCompileTimeEqualTests<u64>();
    }
    SECTION("Scalar - f64")
    {
        RunCompileTimeEqualTests<f64>();
        RunCompileTimeEqualEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeEqualTests<f32>();
        RunCompileTimeEqualEdgeTests<f32>();
    }

    SECTION("Vec2 - i8")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - u8")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - i16")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - u16")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - i32")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - u32")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - i64")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - u64")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - f64")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - i8")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - u8")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - i16")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - u16")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - i32")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - u32")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - i64")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - u64")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - f64")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - i8")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - u8")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - i16")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - u16")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - i32")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - u32")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - i64")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - u64")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - f64")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Mat3 - f64")
    {
        RunCompileTimeEqualMatTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - f32")
    {
        RunCompileTimeEqualMatTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - f64")
    {
        RunCompileTimeEqualMatTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - f32")
    {
        RunCompileTimeEqualMatTests<TRAP::Math::Mat4f>();
    }

    SECTION("Quat - f64")
    {
        RunCompileTimeEqualQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - f32")
    {
        RunCompileTimeEqualQuatTests<TRAP::Math::Quatf>();
    }
}