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
    consteval void RunCompileTimeGreaterThanTests()
    {
        constexpr T A(T(1.0f));
        constexpr T B(T(0.0f));

        static_assert( TRAP::Math::GreaterThan(A, B));
        static_assert(!TRAP::Math::GreaterThan(B, A));

        if constexpr(std::floating_point<T> || std::signed_integral<T>)
        {
            constexpr T C(T(-1.0f));

            static_assert(TRAP::Math::GreaterThan(B, C));
            static_assert(!TRAP::Math::GreaterThan(C, B));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T>
    consteval void RunCompileTimeGreaterThanVecTests()
    {
        constexpr T A(TRAP::Math::Vec<4, typename T::value_type>(2, 3, 4, 5));
        constexpr T B(TRAP::Math::Vec<4, typename T::value_type>(1, 2, 3, 4));

        static_assert( TRAP::Math::All(TRAP::Math::GreaterThan(A, B)));
        static_assert(!TRAP::Math::All(TRAP::Math::GreaterThan(B, A)));

        if constexpr(std::floating_point<typename T::value_type> || std::signed_integral<typename T::value_type>)
        {
            constexpr T C(TRAP::Math::Vec<4, typename T::value_type>(-2, -3, -4, -5));

            static_assert( TRAP::Math::All(TRAP::Math::GreaterThan(A, C)));
            static_assert(!TRAP::Math::All(TRAP::Math::GreaterThan(C, A)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    consteval void RunCompileTimeGreaterThanQuatTests()
    {
        constexpr T A(5.0f, 5.0f, 5.0f, 5.0f);
        constexpr T B(0.0f, 0.0f, 0.0f, 0.0f);
        constexpr T C(-1.0f, -1.0f, -1.0f, -1.0f);

        static_assert( TRAP::Math::All(TRAP::Math::GreaterThan(A, B)));
        static_assert(!TRAP::Math::All(TRAP::Math::GreaterThan(B, A)));
        static_assert( TRAP::Math::All(TRAP::Math::GreaterThan(A, C)));
        static_assert(!TRAP::Math::All(TRAP::Math::GreaterThan(C, A)));
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunCompileTimeGreaterThanEdgeTests()
    {
        constexpr T max = std::numeric_limits<T>::max();
        constexpr T min = std::numeric_limits<T>::lowest();
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();

        static_assert(!TRAP::Math::GreaterThan(max, max));
        static_assert(!TRAP::Math::GreaterThan(min, min));
        static_assert( TRAP::Math::GreaterThan(max, min));
        static_assert(!TRAP::Math::GreaterThan(min, max));
        static_assert(!TRAP::Math::GreaterThan(nan, nan));
        static_assert(!TRAP::Math::GreaterThan(inf, inf));
        static_assert(!TRAP::Math::GreaterThan(ninf, ninf));
    }
}

TEST_CASE("TRAP::Math::GreaterThan()", "[math][generic][greaterthan]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeGreaterThanTests<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeGreaterThanTests<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeGreaterThanTests<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeGreaterThanTests<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeGreaterThanTests<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeGreaterThanTests<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeGreaterThanTests<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeGreaterThanTests<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeGreaterThanTests<double>();
        RunCompileTimeGreaterThanEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeGreaterThanTests<float>();
        RunCompileTimeGreaterThanEdgeTests<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeGreaterThanVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunCompileTimeGreaterThanQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunCompileTimeGreaterThanQuatTests<TRAP::Math::Quatf>();
    }
}