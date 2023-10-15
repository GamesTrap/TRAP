#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunTanCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Tan(T(-1.5)), T(-14.1014), T(0.0001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Tan(T(0.0)), T(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Tan(T(0.001)), T(0.001), T(0.000000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Tan(T(1.001)), T(1.56084), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Tan(T(1.5)), T(14.1014), T(0.0001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Tan(T(11.1)), T(-9.54135), T(0.0001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Tan(T(50.0)), T(-0.271901), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Tan(T(150.0)), T(-1.02235), T(0.0001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunTanRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 8> values
        {
            T(-1.5), T(0.0), T(0.001), T(1.001), T(1.5), T(11.1), T(50.0), T(150.0)
        };

        for(const T val : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Tan(val), std::tan(val), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunTanEdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();
        static constexpr T inf = std::numeric_limits<T>::infinity();
        static constexpr T ninf = -std::numeric_limits<T>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Tan(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Tan(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Tan(ninf)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunTanVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Tan(T(typename T::value_type(-1.5f))), T(-14.1014f), T(0.0001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Tan(T(typename T::value_type(0.0f))), T(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Tan(T(typename T::value_type(0.001f))), T(0.001f), T(0.000000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Tan(T(typename T::value_type(1.001f))), T(1.56084f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Tan(T(typename T::value_type(1.5f))), T(14.1014f), T(0.0001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Tan(T(typename T::value_type(11.1f))), T(-9.54135f), T(0.0001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Tan(T(typename T::value_type(50.0f))), T(-0.271901f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::Tan(T(typename T::value_type(150.0f))), T(-1.02235f), T(0.0001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunTanVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<T, 8> values
        {
            T(typename T::value_type(-1.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)), T(typename T::value_type(1.001)),
            T(typename T::value_type(1.5)), T(typename T::value_type(11.1)), T(typename T::value_type(50.0)), T(typename T::value_type(150.0))
        };

        for(const T val : values)
        {
            const T s = TRAP::Math::Tan(val);
            for(uint32_t l = 0; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::tan(val[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunTanVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Tan(T(nan)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Tan(T(inf)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::Tan(T(ninf)))));
    }
}

TEST_CASE("TRAP::Math::Tan()", "[math][generic][tan]")
{
    SECTION("Scalar - double")
    {
        RunTanRunTimeTests<double>();
        RunTanCompileTimeTests<double>();
        RunTanEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunTanRunTimeTests<float>();
        RunTanCompileTimeTests<float>();
        RunTanEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunTanVecRunTimeTests<TRAP::Math::Vec2d>();
        RunTanVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunTanVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunTanVecRunTimeTests<TRAP::Math::Vec2f>();
        RunTanVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunTanVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunTanVecRunTimeTests<TRAP::Math::Vec3d>();
        RunTanVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunTanVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunTanVecRunTimeTests<TRAP::Math::Vec3f>();
        RunTanVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunTanVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunTanVecRunTimeTests<TRAP::Math::Vec4d>();
        RunTanVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunTanVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunTanVecRunTimeTests<TRAP::Math::Vec4f>();
        RunTanVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunTanVecEdgeTests<TRAP::Math::Vec4f>();
    }
}
