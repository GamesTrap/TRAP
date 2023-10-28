#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::signed_integral<T> || std::floating_point<T>
    consteval void RunCompileTimeAbsEdgeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr T max = std::numeric_limits<T>::max();
        static_assert(TRAP::Math::Equal(TRAP::Math::Abs(max), max, Epsilon));
        constexpr T nmax = -std::numeric_limits<T>::max();
        static_assert(TRAP::Math::Equal(TRAP::Math::Abs(nmax), max, Epsilon));
        if constexpr(std::floating_point<T>)
        {
            constexpr T min = std::numeric_limits<T>::lowest();
            static_assert(TRAP::Math::Abs(min) == max);
            constexpr T nan = std::numeric_limits<T>::quiet_NaN();
            static_assert(TRAP::Math::IsNaN(TRAP::Math::Abs(nan)));
            constexpr T inf = std::numeric_limits<T>::infinity();
            static_assert(TRAP::Math::Abs(inf) == inf);
            constexpr T ninf = -std::numeric_limits<T>::infinity();
            static_assert(TRAP::Math::Abs(ninf) == inf);
            constexpr T dmin = std::numeric_limits<T>::denorm_min();
            static_assert(TRAP::Math::Abs(dmin) == dmin);
        }
    }

    template<typename T>
    requires std::signed_integral<T> || std::floating_point<T>
    consteval void RunCompileTimeAbsTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(0.0)), static_cast<T>(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(-0.0)), static_cast<T>(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(1.0)), static_cast<T>(1.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(-1.0)), static_cast<T>(1.0), Epsilon));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && (std::signed_integral<typename T::value_type> || std::floating_point<typename T::value_type>)
    consteval void RunCompileTimeAbsVecTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::value_type>(0.0))), T(static_cast<T::value_type>(0.0)), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::value_type>(-0.0))), T(static_cast<T::value_type>(0.0)), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::value_type>(1.0))), T(static_cast<T::value_type>(1.0)), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::value_type>(-1.0))), T(static_cast<T::value_type>(1.0)), Epsilon)));
    }

    template<typename T>
    requires TRAP::Math::IsMat3<T>
    consteval void RunCompileTimeAbsMat3Tests()
    {
        constexpr T A
        (
            typename T::value_type(3.0), typename T::value_type(1.0), typename T::value_type(5.2),
            typename T::value_type(1.4), typename T::value_type(0.5), typename T::value_type(9.3),
            typename T::value_type(6.8), typename T::value_type(8.4), typename T::value_type(4.3)
        );
        constexpr T B
        (
            typename T::value_type( 1.0), typename T::value_type(-1.0), typename T::value_type( 1.0),
            typename T::value_type(-1.0), typename T::value_type( 1.0), typename T::value_type( 1.0),
            typename T::value_type( 1.0), typename T::value_type(-1.0), typename T::value_type(-1.0)
        );

        constexpr T C = TRAP::Math::MatrixCompMult(A, B);
        constexpr T D = TRAP::Math::Abs(C);

        constexpr TRAP::Math::Vec3b col1 = TRAP::Math::Equal(std::get<0>(D), std::get<0>(A));
        constexpr TRAP::Math::Vec3b col2 = TRAP::Math::Equal(std::get<1>(D), std::get<1>(A));
        constexpr TRAP::Math::Vec3b col3 = TRAP::Math::Equal(std::get<2>(D), std::get<2>(A));

        static_assert(TRAP::Math::All(TRAP::Math::Vec3b{TRAP::Math::All(col1),
                                                        TRAP::Math::All(col2),
                                                        TRAP::Math::All(col3)}));
    }

    template<typename T>
    requires TRAP::Math::IsMat4<T>
    consteval void RunCompileTimeAbsMat4Tests()
    {
        constexpr T A
        (
            typename T::value_type(3.0), typename T::value_type(1.0), typename T::value_type(5.2), typename T::value_type(4.9),
            typename T::value_type(1.4), typename T::value_type(0.5), typename T::value_type(9.3), typename T::value_type(3.7),
            typename T::value_type(6.8), typename T::value_type(8.4), typename T::value_type(4.3), typename T::value_type(3.9),
            typename T::value_type(5.6), typename T::value_type(7.2), typename T::value_type(1.1), typename T::value_type(4.4)
        );
        constexpr T B
        (
            typename T::value_type( 1.0), typename T::value_type(-1.0), typename T::value_type( 1.0), typename T::value_type( 1.0),
            typename T::value_type(-1.0), typename T::value_type( 1.0), typename T::value_type( 1.0), typename T::value_type(-1.0),
            typename T::value_type( 1.0), typename T::value_type(-1.0), typename T::value_type(-1.0), typename T::value_type(-1.0),
            typename T::value_type(-1.0), typename T::value_type(-1.0), typename T::value_type( 1.0), typename T::value_type( 1.0)
        );

        constexpr T C = TRAP::Math::MatrixCompMult(A, B);
        constexpr T D = TRAP::Math::Abs(C);

        constexpr TRAP::Math::Vec4b col1 = TRAP::Math::Equal(std::get<0>(D), std::get<0>(A));
        constexpr TRAP::Math::Vec4b col2 = TRAP::Math::Equal(std::get<1>(D), std::get<1>(A));
        constexpr TRAP::Math::Vec4b col3 = TRAP::Math::Equal(std::get<2>(D), std::get<2>(A));
        constexpr TRAP::Math::Vec4b col4 = TRAP::Math::Equal(std::get<3>(D), std::get<3>(A));

        static_assert(TRAP::Math::All(TRAP::Math::Vec4b{TRAP::Math::All(col1),
                                                        TRAP::Math::All(col2),
                                                        TRAP::Math::All(col3),
                                                        TRAP::Math::All(col4)}));
    }
}

TEST_CASE("TRAP::Math::Abs()", "[math][generic][abs]")
{
    SECTION("Scalar - i8")
    {
        RunCompileTimeAbsEdgeTests<i8>();
        RunCompileTimeAbsTests<i8>();
    }
    SECTION("Scalar - i16")
    {
        RunCompileTimeAbsEdgeTests<i16>();
        RunCompileTimeAbsTests<i16>();
    }
    SECTION("Scalar - i32")
    {
        RunCompileTimeAbsEdgeTests<i32>();
        RunCompileTimeAbsTests<i32>();
    }
    SECTION("Scalar - i64")
    {
        RunCompileTimeAbsEdgeTests<i64>();
        RunCompileTimeAbsTests<i64>();
    }
    SECTION("Scalar - f64")
    {
        RunCompileTimeAbsEdgeTests<f64>();
        RunCompileTimeAbsTests<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeAbsEdgeTests<f32>();
        RunCompileTimeAbsTests<f32>();
    }

    SECTION("Vec2 - i8")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - i16")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - i32")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - i64")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - f64")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - i8")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - i16")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - i32")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - i64")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - f64")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - i8")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - i16")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - i32")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - i64")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - f64")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Mat3 - f64")
    {
        RunCompileTimeAbsMat3Tests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - f32")
    {
        RunCompileTimeAbsMat3Tests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - f64")
    {
        RunCompileTimeAbsMat4Tests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - f32")
    {
        RunCompileTimeAbsMat4Tests<TRAP::Math::Mat4f>();
    }
}