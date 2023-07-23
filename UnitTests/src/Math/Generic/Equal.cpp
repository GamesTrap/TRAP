#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

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
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

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

    if constexpr(std::floating_point<typename T::valueType> || std::signed_integral<typename T::valueType>)
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
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

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

    static_assert(TRAP::Math::All(TRAP::Math::Equal(A, A, typename T::colType(Epsilon))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(B, B, typename T::colType(Epsilon))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(C, C, typename T::colType(Epsilon))));
    static_assert(TRAP::Math::All(TRAP::Math::Equal(D, D, typename T::colType(Epsilon))));
    static_assert(!TRAP::Math::All(TRAP::Math::Equal(A, B, typename T::colType(Epsilon))));
    static_assert(!TRAP::Math::All(TRAP::Math::Equal(B, A, typename T::colType(Epsilon))));
}

template<typename T>
requires TRAP::Math::IsQuat<T>
consteval void RunCompileTimeEqualQuatTests()
{
    constexpr typename T::valueType Epsilon = std::numeric_limits<typename T::valueType>::epsilon();

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

TEST_CASE("TRAP::Math::Equal()", "[math][generic][equal]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeEqualTests<int8_t>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeEqualTests<uint8_t>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeEqualTests<int16_t>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeEqualTests<uint16_t>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeEqualTests<int32_t>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeEqualTests<uint32_t>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeEqualTests<int64_t>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeEqualTests<uint64_t>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeEqualTests<double>();
        RunCompileTimeEqualEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeEqualTests<float>();
        RunCompileTimeEqualEdgeTests<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2i8>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2ui8>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2i16>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2ui16>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2i32>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2ui32>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2i64>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2ui64>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3i8>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3ui8>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3i16>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3ui16>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3i32>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3ui32>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3i64>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3ui64>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4i8>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4ui8>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4i16>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4ui16>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4i32>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4ui32>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4i64>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4ui64>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeEqualVecTests<TRAP::Math::Vec4f>();
    }

    SECTION("Mat3 - double")
    {
        RunCompileTimeEqualMatTests<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunCompileTimeEqualMatTests<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunCompileTimeEqualMatTests<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeEqualMatTests<TRAP::Math::Mat4f>();
    }

    SECTION("Quat - double")
    {
        RunCompileTimeEqualQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunCompileTimeEqualQuatTests<TRAP::Math::Quatf>();
    }
}