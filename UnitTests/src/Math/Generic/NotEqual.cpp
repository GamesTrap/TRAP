#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunCompileTimeNotEqualTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr T A(T(0.0f));
        constexpr T B(T(5.0f));

        static_assert(!TRAP::Math::NotEqual(A, A));
        static_assert(!TRAP::Math::NotEqual(B, B));
        static_assert( TRAP::Math::NotEqual(A, B));
        static_assert( TRAP::Math::NotEqual(B, A));

        static_assert(!TRAP::Math::NotEqual(A, A, Epsilon));
        static_assert(!TRAP::Math::NotEqual(B, B, Epsilon));
        static_assert( TRAP::Math::NotEqual(A, B, Epsilon));
        static_assert( TRAP::Math::NotEqual(B, A, Epsilon));

        if constexpr(std::floating_point<T> || std::signed_integral<T>)
        {
            constexpr T C(T(-1.0f));
            constexpr T D(T(-5.0f));

            static_assert(!TRAP::Math::NotEqual(C, C));
            static_assert(!TRAP::Math::NotEqual(D, D));
            static_assert( TRAP::Math::NotEqual(C, D));
            static_assert( TRAP::Math::NotEqual(D, C));

            static_assert(!TRAP::Math::NotEqual(C, C, Epsilon));
            static_assert(!TRAP::Math::NotEqual(D, D, Epsilon));
            static_assert( TRAP::Math::NotEqual(C, D, Epsilon));
            static_assert( TRAP::Math::NotEqual(D, C, Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T>
    consteval void RunCompileTimeNotEqualVecTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        constexpr T A(T(0));
        constexpr T B(T(5));

        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(A, A)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(B, B)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(A, B)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(B, A)));

        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(A, A, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(B, B, Epsilon)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(A, B, Epsilon)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(B, A, Epsilon)));

        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(A, A, T(Epsilon))));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(B, B, T(Epsilon))));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(A, B, T(Epsilon))));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(B, A, T(Epsilon))));

        if constexpr(std::floating_point<typename T::value_type> || std::signed_integral<typename T::value_type>)
        {
            constexpr T C(T(-1));
            constexpr T D(T(-5));

            static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(C, C)));
            static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(D, D)));
            static_assert( TRAP::Math::All(TRAP::Math::NotEqual(C, D)));
            static_assert( TRAP::Math::All(TRAP::Math::NotEqual(D, C)));

            static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(C, C, Epsilon)));
            static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(D, D, Epsilon)));
            static_assert( TRAP::Math::All(TRAP::Math::NotEqual(C, D, Epsilon)));
            static_assert( TRAP::Math::All(TRAP::Math::NotEqual(D, C, Epsilon)));

            static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(C, C, T(Epsilon))));
            static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(D, D, T(Epsilon))));
            static_assert( TRAP::Math::All(TRAP::Math::NotEqual(C, D, T(Epsilon))));
            static_assert( TRAP::Math::All(TRAP::Math::NotEqual(D, C, T(Epsilon))));
        }
    }

    template<typename T>
    requires TRAP::Math::IsMat<T>
    consteval void RunCompileTimeNotEqualMatTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        constexpr T A(0.0f);
        constexpr T B(5.0f);
        constexpr T C(-1.0f);
        constexpr T D(-5.0f);

        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(A, A)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(B, B)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(C, C)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(D, D)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(A, B)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(B, A)));

        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(A, A, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(B, B, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(C, C, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(D, D, Epsilon)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(A, B, Epsilon)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(B, A, Epsilon)));

        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(A, A, typename T::col_type(Epsilon))));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(B, B, typename T::col_type(Epsilon))));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(C, C, typename T::col_type(Epsilon))));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(D, D, typename T::col_type(Epsilon))));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(A, B, typename T::col_type(Epsilon))));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(B, A, typename T::col_type(Epsilon))));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    consteval void RunCompileTimeNotEqualQuatTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        constexpr T A(0.0f, 0.0f, 0.0f, 0.0f);
        constexpr T B(5.0f, 5.0f, 5.0f, 5.0f);
        constexpr T C(-1.0f, -1.0f, -1.0f, -1.0f);
        constexpr T D(-5.0f, -5.0f, -5.0f, -5.0f);

        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(A, A)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(B, B)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(C, C)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(D, D)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(A, B)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(B, A)));

        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(A, A, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(B, B, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(C, C, Epsilon)));
        static_assert(!TRAP::Math::All(TRAP::Math::NotEqual(D, D, Epsilon)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(A, B, Epsilon)));
        static_assert( TRAP::Math::All(TRAP::Math::NotEqual(B, A, Epsilon)));
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunCompileTimeNotEqualEdgeTests()
    {
        constexpr T max = std::numeric_limits<T>::max();
        constexpr T min = std::numeric_limits<T>::lowest();
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();

        static_assert(!TRAP::Math::NotEqual(max, max));
        static_assert(!TRAP::Math::NotEqual(min, min));
        static_assert( TRAP::Math::NotEqual(max, min));
        static_assert( TRAP::Math::NotEqual(min, max));
        static_assert( TRAP::Math::NotEqual(nan, nan));
        static_assert(!TRAP::Math::NotEqual(inf, inf));
        static_assert(!TRAP::Math::NotEqual(ninf, ninf));
    }
}

TEST_CASE("TRAP::Math::NotEqual()", "[math][generic][notequal]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeNotEqualTests<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeNotEqualTests<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeNotEqualTests<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeNotEqualTests<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeNotEqualTests<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeNotEqualTests<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeNotEqualTests<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeNotEqualTests<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeNotEqualTests<double>();
        RunCompileTimeNotEqualEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeNotEqualTests<float>();
        RunCompileTimeNotEqualEdgeTests<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeNotEqualVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Mat3 - double")
    {
        RunCompileTimeNotEqualMatTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunCompileTimeNotEqualMatTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunCompileTimeNotEqualMatTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeNotEqualMatTests<TRAP::Math::Mat4f>();
    }

    SECTION("Quat - double")
    {
        RunCompileTimeNotEqualQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunCompileTimeNotEqualQuatTests<TRAP::Math::Quatf>();
    }
}