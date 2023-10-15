#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunExpCompileTimeTests()
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(T(-40.0f)), T(0.0f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(T(- 4.0f)), T(0.0183156393f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(T(0.0001f)), T(1.0001000049976403f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(T(1.0f)), T(2.71828175f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(T(1.75f)), T(5.75460291f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(T(1.9991f)), T(7.382409f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(T(2.1f)), T(8.166169f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(T(4.0f)), T(54.598150033144236f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(T(0.0f)), T(1.0f), T(0.00001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunExpRunTimeTests()
    {
        const std::array<std::tuple<T, T>, 9> values
        {
            std::tuple(T(-40.0f), T(0.0f)),
            std::tuple(T(-4.0f), T(0.0183156393f)),
            std::tuple(T(0.0001f), T(1.0001000049976403f)),
            std::tuple(T(1.0f), T(2.71828175f)),
            std::tuple(T(1.75f), T(5.75460291f)),
            std::tuple(T(1.9991f), T(7.382409f)),
            std::tuple(T(2.1f), T(8.166169f)),
            std::tuple(T(4.0f), T(54.598150033144236f)),
            std::tuple(T(0.0f), T(1.0f)),
        };

        for(const auto& [num, expected] : values)
        {
            const T res = TRAP::Math::Exp(num);
            REQUIRE(TRAP::Math::Equal(res, expected, T(0.00001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunExpVecCompileTimeTests()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(T(1.0f)), T(TRAP::Math::e<typename T::value_type>()), T(0.000001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunExpVecRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(T(1.0f)), T(TRAP::Math::e<typename T::value_type>()), Epsilon)));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    consteval void RunExpQuatCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        constexpr T q(typename T::value_type(1.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        constexpr T res(typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q), res, Epsilon)));

        constexpr T q2(typename T::value_type(0.0f), typename T::value_type(1.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        constexpr T res2(TRAP::Math::Cos(typename T::value_type(1.0f)), TRAP::Math::Sin(typename T::value_type(1.0f)), typename T::value_type(0.0f), typename T::value_type(0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q2), res2, Epsilon)));

        constexpr T q3(typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(1.0f), typename T::value_type(0.0f));
        constexpr T res3(TRAP::Math::Cos(typename T::value_type(1.0f)), typename T::value_type(0.0f), TRAP::Math::Sin(typename T::value_type(1.0f)), typename T::value_type(0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q3), res3, Epsilon)));

        constexpr T q4(typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(1.0f));
        constexpr T res4(TRAP::Math::Cos(typename T::value_type(1.0f)), typename T::value_type(0.0f), typename T::value_type(0.0f), TRAP::Math::Sin(typename T::value_type(1.0f)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q4), res4, Epsilon)));

        constexpr T q6(typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q6), q6, Epsilon)));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    void RunExpQuatRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        const T q(typename T::value_type(1.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        const T res(typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q), res, Epsilon)));

        const T q2(typename T::value_type(0.0f), typename T::value_type(1.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        const T res2(TRAP::Math::Cos(typename T::value_type(1.0f)), TRAP::Math::Sin(typename T::value_type(1.0f)), typename T::value_type(0.0f), typename T::value_type(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q2), res2, Epsilon)));

        const T q3(typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(1.0f), typename T::value_type(0.0f));
        const T res3(TRAP::Math::Cos(typename T::value_type(1.0f)), typename T::value_type(0.0f), TRAP::Math::Sin(typename T::value_type(1.0f)), typename T::value_type(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q3), res3, Epsilon)));

        const T q4(typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(1.0f));
        const T res4(TRAP::Math::Cos(typename T::value_type(1.0f)), typename T::value_type(0.0f), typename T::value_type(0.0f), TRAP::Math::Sin(typename T::value_type(1.0f)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q4), res4, Epsilon)));

        const T q6(typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Exp(q6), q6, Epsilon)));
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunExpEdgeCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();

        static_assert(TRAP::Math::Equal(TRAP::Math::Exp(ninf), T(0.0f), Epsilon));
        static_assert(TRAP::Math::IsInf(TRAP::Math::Exp(inf)));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Exp(nan)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunExpEdgeRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T max = std::numeric_limits<T>::max();
        static constexpr T min = std::numeric_limits<T>::lowest();

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp(ninf), T(0.0f), Epsilon));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Exp(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Exp(nan)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Exp(max)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Exp(min), T(0.0f), Epsilon));
    }
}

TEST_CASE("TRAP::Math::Exp()", "[math][generic][exp]")
{
    SECTION("Scalar - double")
    {
        RunExpRunTimeTests<double>();
        RunExpCompileTimeTests<double>();
        RunExpEdgeRunTimeTests<double>();
        RunExpEdgeCompileTimeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunExpRunTimeTests<float>();
        RunExpCompileTimeTests<float>();
        RunExpEdgeRunTimeTests<float>();
        RunExpEdgeCompileTimeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunExpVecRunTimeTests<TRAP::Math::Vec2d>();
        RunExpVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunExpVecRunTimeTests<TRAP::Math::Vec2f>();
        RunExpVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunExpVecRunTimeTests<TRAP::Math::Vec3d>();
        RunExpVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunExpVecRunTimeTests<TRAP::Math::Vec3f>();
        RunExpVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunExpVecRunTimeTests<TRAP::Math::Vec4d>();
        RunExpVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunExpVecRunTimeTests<TRAP::Math::Vec4f>();
        RunExpVecCompileTimeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunExpQuatRunTimeTests<TRAP::Math::Quatd>();
        RunExpQuatCompileTimeTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunExpQuatRunTimeTests<TRAP::Math::Quatf>();
        RunExpQuatCompileTimeTests<TRAP::Math::Quatf>();
    }
}