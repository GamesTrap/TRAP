#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunSqrtCompileTimeTests()
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Sqrt(T(0.5f)), T(0.707107f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sqrt(T(1.5f)), T(1.22474f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sqrt(T(2.0f)), T(1.41421f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sqrt(T(41.5f)), T(6.44205f), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Sqrt(T(0.0f)), T(0.0f), T(0.00001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunSqrtRunTimeTests()
    {
        static constexpr std::array<std::tuple<T, T>, 5> values
        {
            std::tuple(T(0.5f), T(0.707107f)),
            std::tuple(T(1.5f), T(1.22474f)),
            std::tuple(T(2.0f), T(1.41421f)),
            std::tuple(T(41.5f), T(6.44205f)),
            std::tuple(T(0.0f), T(0.0f)),
        };

        for(const auto& [x, expected] : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Sqrt(x), expected, T(0.00001f)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunSqrtVecCompileTimeTests()
    {
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(T(4.0f)), T(2.0f), T(0.01f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunSqrtVecRunTimeTests()
    {
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(T(4.0f)), T(2.0f), T(0.01f))));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    consteval void RunSqrtQuatCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = typename T::value_type(0.001f);

        {
            constexpr T x(typename T::value_type(1.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
            constexpr T res(typename T::value_type(1.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
        {
            constexpr T x(typename T::value_type(2.0f), typename T::value_type(3.0f), typename T::value_type(4.0f), typename T::value_type(5.0f));
            constexpr T res(typename T::value_type(2.162f), typename T::value_type(0.693803f), typename T::value_type(0.92507f), typename T::value_type(1.15634f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
        {
            constexpr T x(typename T::value_type(-2.0f), typename T::value_type(-3.0f), typename T::value_type(-4.0f), typename T::value_type(-5.0f));
            constexpr T res(typename T::value_type(1.63531f), typename T::value_type(-0.917258f), typename T::value_type(-1.22301f), typename T::value_type(-1.52876f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    void RunSqrtQuatRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = typename T::value_type(0.001f);

        {
            static constexpr T x(typename T::value_type(1.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
            static constexpr T res(typename T::value_type(1.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
        {
            static constexpr T x(typename T::value_type(2.0f), typename T::value_type(3.0f), typename T::value_type(4.0f), typename T::value_type(5.0f));
            static constexpr T res(typename T::value_type(2.162f), typename T::value_type(0.693803f), typename T::value_type(0.92507f), typename T::value_type(1.15634f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
        {
            static constexpr T x(typename T::value_type(-2.0f), typename T::value_type(-3.0f), typename T::value_type(-4.0f), typename T::value_type(-5.0f));
            static constexpr T res(typename T::value_type(1.63531f), typename T::value_type(-0.917258f), typename T::value_type(-1.22301f), typename T::value_type(-1.52876f));
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Sqrt(x), res, Epsilon)));
        }
        {
            static constexpr T x(typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f), typename T::value_type(0.0f));
            REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Sqrt(x))));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    consteval void RunSqrtEdgeCompileTimeTests()
    {
        constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        constexpr T inf = std::numeric_limits<T>::infinity();
        constexpr T ninf = -std::numeric_limits<T>::infinity();
        constexpr T min = std::numeric_limits<T>::lowest();

        static_assert(TRAP::Math::IsNaN(TRAP::Math::Sqrt(min)));
        static_assert(TRAP::Math::IsInf(TRAP::Math::Sqrt(inf)));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Sqrt(ninf)));
        static_assert(TRAP::Math::IsNaN(TRAP::Math::Sqrt(nan)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunSqrtEdgeRunTimeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();
        static constexpr T min = std::numeric_limits<T>::lowest();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sqrt(min)));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Sqrt(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sqrt(ninf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Sqrt(nan)));
    }
}

TEST_CASE("TRAP::Math::Sqrt()", "[math][generic][sqrt]")
{
    SECTION("Scalar - double")
    {
        RunSqrtRunTimeTests<double>();
        RunSqrtCompileTimeTests<double>();
        RunSqrtEdgeRunTimeTests<double>();
        RunSqrtEdgeCompileTimeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunSqrtRunTimeTests<float>();
        RunSqrtCompileTimeTests<float>();
        RunSqrtEdgeRunTimeTests<float>();
        RunSqrtEdgeCompileTimeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunSqrtVecRunTimeTests<TRAP::Math::Vec2d>();
        RunSqrtVecCompileTimeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunSqrtVecRunTimeTests<TRAP::Math::Vec2f>();
        RunSqrtVecCompileTimeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunSqrtVecRunTimeTests<TRAP::Math::Vec3d>();
        RunSqrtVecCompileTimeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunSqrtVecRunTimeTests<TRAP::Math::Vec3f>();
        RunSqrtVecCompileTimeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunSqrtVecRunTimeTests<TRAP::Math::Vec4d>();
        RunSqrtVecCompileTimeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunSqrtVecRunTimeTests<TRAP::Math::Vec4f>();
        RunSqrtVecCompileTimeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - double")
    {
        RunSqrtQuatRunTimeTests<TRAP::Math::Quatd>();
        RunSqrtQuatCompileTimeTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunSqrtQuatRunTimeTests<TRAP::Math::Quatf>();
        RunSqrtQuatCompileTimeTests<TRAP::Math::Quatf>();
    }
}