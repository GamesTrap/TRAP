#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::is_arithmetic_v<T>
    consteval void RunCompileTimeClampScalarTest()
    {
        constexpr T N = static_cast<T>(1);
        constexpr T B = static_cast<T>(10);

        static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N, B), T(5), TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N, B), T(1), TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N, B), T(10), TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N, B), T(1), TRAP::Math::Epsilon<T>()));
        static_assert(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N, B), T(10), TRAP::Math::Epsilon<T>()));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T>
    consteval void RunCompileTimeClampVecTest()
    {
        constexpr T N = static_cast<T>(1);
        constexpr T B = static_cast<T>(10);

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N, B), T(5), TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N, B), T(1), TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N, B), T(10), TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N, B), T(1), TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N, B), T(10), TRAP::Math::Epsilon<T>())));

        constexpr typename T::value_type N1 = static_cast<T::value_type>(1);
        constexpr typename T::value_type B1 = static_cast<T::value_type>(10);

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N1, B1), T(5), TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N1, B1), T(1), TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N1, B1), T(10), TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N1, B1), T(1), TRAP::Math::Epsilon<T>())));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N1, B1), T(10), TRAP::Math::Epsilon<T>())));
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunClampEdgeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::lowest();
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(max, min, max), max, Epsilon));
        if constexpr(std::floating_point<T>)
        {
            REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Clamp(nan, T(0.0), T(1.0))));
            STATIC_REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(T(5.0), nan, nan), T(5.0), Epsilon));
        }
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void RunRunTimeClampScalarTest()
    {
        T N = static_cast<T>(1);
        T B = static_cast<T>(10);

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N, B), T(5), TRAP::Math::Epsilon<T>()));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N, B), T(1), TRAP::Math::Epsilon<T>()));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N, B), T(10), TRAP::Math::Epsilon<T>()));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N, B), T(1), TRAP::Math::Epsilon<T>()));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N, B), T(10), TRAP::Math::Epsilon<T>()));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T>
    void RunRunTimeClampVecTest()
    {
        T N = static_cast<T>(1);
        T B = static_cast<T>(10);

        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N, B), T(5), TRAP::Math::Epsilon<T>())));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N, B), T(1), TRAP::Math::Epsilon<T>())));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N, B), T(10), TRAP::Math::Epsilon<T>())));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N, B), T(1), TRAP::Math::Epsilon<T>())));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N, B), T(10), TRAP::Math::Epsilon<T>())));

        typename T::value_type N1 = static_cast<T::value_type>(1);
        typename T::value_type B1 = static_cast<T::value_type>(10);

        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(5), N1, B1), T(5), TRAP::Math::Epsilon<T>())));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(0), N1, B1), T(1), TRAP::Math::Epsilon<T>())));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(11), N1, B1), T(10), TRAP::Math::Epsilon<T>())));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(1), N1, B1), T(1), TRAP::Math::Epsilon<T>())));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Clamp(T(10), N1, B1), T(10), TRAP::Math::Epsilon<T>())));
    }
}

TEST_CASE("TRAP::Math::Clamp()", "[math][generic][clamp]")
{
    SECTION("Scalar - i8")
    {
        RunCompileTimeClampScalarTest<i8>();
        RunRunTimeClampScalarTest<i8>();
        RunClampEdgeTests<i8>();
    }
    SECTION("Scalar - u8")
    {
        RunCompileTimeClampScalarTest<u8>();
        RunRunTimeClampScalarTest<u8>();
        RunClampEdgeTests<u8>();
    }
    SECTION("Scalar - i16")
    {
        RunCompileTimeClampScalarTest<i16>();
        RunRunTimeClampScalarTest<i16>();
        RunClampEdgeTests<i16>();
    }
    SECTION("Scalar - u16")
    {
        RunCompileTimeClampScalarTest<u16>();
        RunRunTimeClampScalarTest<u16>();
        RunClampEdgeTests<u16>();
    }
    SECTION("Scalar - i32")
    {
        RunCompileTimeClampScalarTest<i32>();
        RunRunTimeClampScalarTest<i32>();
        RunClampEdgeTests<i32>();
    }
    SECTION("Scalar - u32")
    {
        RunCompileTimeClampScalarTest<u32>();
        RunRunTimeClampScalarTest<u32>();
        RunClampEdgeTests<u32>();
    }
    SECTION("Scalar - i64")
    {
        RunCompileTimeClampScalarTest<i64>();
        RunRunTimeClampScalarTest<i64>();
        RunClampEdgeTests<i64>();
    }
    SECTION("Scalar - u64")
    {
        RunCompileTimeClampScalarTest<u64>();
        RunRunTimeClampScalarTest<u64>();
        RunClampEdgeTests<u64>();
    }
    SECTION("Scalar - f64")
    {
        RunCompileTimeClampScalarTest<f64>();
        RunRunTimeClampScalarTest<f64>();
        RunClampEdgeTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeClampScalarTest<f32>();
        RunRunTimeClampScalarTest<f32>();
        RunClampEdgeTests<f32>();
    }

    SECTION("Vec2 - i8")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2i8>();
        RunRunTimeClampVecTest<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - u8")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2ui8>();
        RunRunTimeClampVecTest<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - i16")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2i16>();
        RunRunTimeClampVecTest<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - u16")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2ui16>();
        RunRunTimeClampVecTest<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - i32")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2i32>();
        RunRunTimeClampVecTest<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - u32")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2ui32>();
        RunRunTimeClampVecTest<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - i64")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2i64>();
        RunRunTimeClampVecTest<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - u64")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2ui64>();
        RunRunTimeClampVecTest<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - f64")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2d>();
        RunRunTimeClampVecTest<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec2f>();
        RunRunTimeClampVecTest<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - i8")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3i8>();
        RunRunTimeClampVecTest<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - u8")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3ui8>();
        RunRunTimeClampVecTest<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - i16")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3i16>();
        RunRunTimeClampVecTest<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - u16")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3ui16>();
        RunRunTimeClampVecTest<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - i32")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3i32>();
        RunRunTimeClampVecTest<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - u32")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3ui32>();
        RunRunTimeClampVecTest<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - i64")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3i64>();
        RunRunTimeClampVecTest<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - u64")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3ui64>();
        RunRunTimeClampVecTest<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - f64")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3d>();
        RunRunTimeClampVecTest<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec3f>();
        RunRunTimeClampVecTest<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - i8")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4i8>();
        RunRunTimeClampVecTest<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - u8")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4ui8>();
        RunRunTimeClampVecTest<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - i16")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4i16>();
        RunRunTimeClampVecTest<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - u16")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4ui16>();
        RunRunTimeClampVecTest<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - i32")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4i32>();
        RunRunTimeClampVecTest<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - u32")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4ui32>();
        RunRunTimeClampVecTest<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - i64")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4i64>();
        RunRunTimeClampVecTest<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - u64")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4ui64>();
        RunRunTimeClampVecTest<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - f64")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4d>();
        RunRunTimeClampVecTest<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeClampVecTest<TRAP::Math::Vec4f>();
        RunRunTimeClampVecTest<TRAP::Math::Vec4f>();
    }
}