#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::signed_integral<T> || std::floating_point<T>
void RunAbsTests()
{
    const std::array<T, 4> values
    {
        T(0.0), T(-0.0), T(1.0), T(-1.0)
    };

    constexpr T Epsilon = std::numeric_limits<T>::epsilon();
    for(const T val : values)
    {
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Abs(val), static_cast<T>(std::abs(val)), Epsilon));
    }
}

template<typename T>
requires std::signed_integral<T> || std::floating_point<T>
constexpr void RunConstexprAbsTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(0.0)), static_cast<T>(0.0), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(-0.0)), static_cast<T>(0.0), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(1.0)), static_cast<T>(1.0), Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::Abs(static_cast<T>(-1.0)), static_cast<T>(1.0), Epsilon));
}

template<typename T>
requires TRAP::Math::IsVec<T> && (std::signed_integral<typename T::valueType> || std::floating_point<typename T::valueType>)
constexpr void RunConstexprAbsVecTests()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(0.0))), T(static_cast<T::valueType>(0.0)), Epsilon)));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(-0.0))), T(static_cast<T::valueType>(0.0)), Epsilon)));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(1.0))), T(static_cast<T::valueType>(1.0)), Epsilon)));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Abs(T(static_cast<T::valueType>(-1.0))), T(static_cast<T::valueType>(1.0)), Epsilon)));
}

template<typename T>
requires TRAP::Math::IsMat3<T>
constexpr void RunConstexprAbsMat3Tests()
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
constexpr void RunConstexprAbsMat4Tests()
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
    SECTION("Scalar - int16_t")
    {
        RunAbsTests<int16_t>();
        RunConstexprAbsTests<int16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunAbsTests<int32_t>();
        RunConstexprAbsTests<int32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunAbsTests<int64_t>();
        RunConstexprAbsTests<int64_t>();
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
    SECTION("Vec2 - int16_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec2i64>();
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
    SECTION("Vec3 - int16_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec3i64>();
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
    SECTION("Vec4 - int16_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - double")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunConstexprAbsVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Mat3 - double")
    {
        RunConstexprAbsMat3Tests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunConstexprAbsMat3Tests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunConstexprAbsMat4Tests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunConstexprAbsMat4Tests<TRAP::Math::Mat4f>();
    }
}