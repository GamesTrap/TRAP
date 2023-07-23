#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

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
requires TRAP::Math::IsVec<T> && (std::signed_integral<typename T::valueType> || std::floating_point<typename T::valueType>)
consteval void RunCompileTimeAbsVecTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(0.0))), T(static_cast<T::valueType>(0.0)), Epsilon)));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(-0.0))), T(static_cast<T::valueType>(0.0)), Epsilon)));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(1.0))), T(static_cast<T::valueType>(1.0)), Epsilon)));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(-1.0))), T(static_cast<T::valueType>(1.0)), Epsilon)));
}

template<typename T>
requires TRAP::Math::IsMat3<T>
consteval void RunCompileTimeAbsMat3Tests()
{
    constexpr T A
    (
        typename T::valueType(3.0), typename T::valueType(1.0), typename T::valueType(5.2),
        typename T::valueType(1.4), typename T::valueType(0.5), typename T::valueType(9.3),
        typename T::valueType(6.8), typename T::valueType(8.4), typename T::valueType(4.3)
    );
    constexpr T B
    (
        typename T::valueType( 1.0), typename T::valueType(-1.0), typename T::valueType( 1.0),
        typename T::valueType(-1.0), typename T::valueType( 1.0), typename T::valueType( 1.0),
        typename T::valueType( 1.0), typename T::valueType(-1.0), typename T::valueType(-1.0)
    );

    constexpr T C = TRAP::Math::MatrixCompMult(A, B);
    constexpr T D = TRAP::Math::Abs(C);

    constexpr TRAP::Math::Vec3b col1 = TRAP::Math::Equal(D[0], A[0]);
    constexpr TRAP::Math::Vec3b col2 = TRAP::Math::Equal(D[1], A[1]);
    constexpr TRAP::Math::Vec3b col3 = TRAP::Math::Equal(D[2], A[2]);

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
        typename T::valueType(3.0), typename T::valueType(1.0), typename T::valueType(5.2), typename T::valueType(4.9),
        typename T::valueType(1.4), typename T::valueType(0.5), typename T::valueType(9.3), typename T::valueType(3.7),
        typename T::valueType(6.8), typename T::valueType(8.4), typename T::valueType(4.3), typename T::valueType(3.9),
        typename T::valueType(5.6), typename T::valueType(7.2), typename T::valueType(1.1), typename T::valueType(4.4)
    );
    constexpr T B
    (
        typename T::valueType( 1.0), typename T::valueType(-1.0), typename T::valueType( 1.0), typename T::valueType( 1.0),
        typename T::valueType(-1.0), typename T::valueType( 1.0), typename T::valueType( 1.0), typename T::valueType(-1.0),
        typename T::valueType( 1.0), typename T::valueType(-1.0), typename T::valueType(-1.0), typename T::valueType(-1.0),
        typename T::valueType(-1.0), typename T::valueType(-1.0), typename T::valueType( 1.0), typename T::valueType( 1.0)
    );

    constexpr T C = TRAP::Math::MatrixCompMult(A, B);
    constexpr T D = TRAP::Math::Abs(C);

    constexpr TRAP::Math::Vec4b col1 = TRAP::Math::Equal(D[0], A[0]);
    constexpr TRAP::Math::Vec4b col2 = TRAP::Math::Equal(D[1], A[1]);
    constexpr TRAP::Math::Vec4b col3 = TRAP::Math::Equal(D[2], A[2]);
    constexpr TRAP::Math::Vec4b col4 = TRAP::Math::Equal(D[3], A[3]);

    static_assert(TRAP::Math::All(TRAP::Math::Vec4b{TRAP::Math::All(col1),
                                                    TRAP::Math::All(col2),
                                                    TRAP::Math::All(col3),
                                                    TRAP::Math::All(col4)}));
}

TEST_CASE("TRAP::Math::Abs()", "[math][generic][abs]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeAbsEdgeTests<int8_t>();
        RunCompileTimeAbsTests<int8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeAbsEdgeTests<int16_t>();
        RunCompileTimeAbsTests<int16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeAbsEdgeTests<int32_t>();
        RunCompileTimeAbsTests<int32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeAbsEdgeTests<int64_t>();
        RunCompileTimeAbsTests<int64_t>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeAbsEdgeTests<double>();
        RunCompileTimeAbsTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeAbsEdgeTests<float>();
        RunCompileTimeAbsTests<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeAbsVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Mat3 - double")
    {
        RunCompileTimeAbsMat3Tests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunCompileTimeAbsMat3Tests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunCompileTimeAbsMat4Tests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeAbsMat4Tests<TRAP::Math::Mat4f>();
    }
}