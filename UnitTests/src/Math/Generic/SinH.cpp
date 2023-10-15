#include <limits>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires std::floating_point<T>
    consteval void RunSinHCompileTimeTests()
    {
        constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::SinH(T(-0.5)), T(-0.521095), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::SinH(T(0.0)), T(0.0), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::SinH(T(0.001)), T(0.001), T(0.0000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::SinH(T(0.5)), T(0.521095), T(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::SinH(T(1.0)), T(1.1752), T(0.00001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::SinH(T(5.0)), T(74.2032), T(0.0001f)));
    }

    template<typename T>
    requires std::floating_point<T>
    void RunSinHRunTimeTests()
    {
        static constexpr T Epsilon = std::numeric_limits<T>::epsilon();

        static constexpr std::array<T, 6> values
        {
            T(-0.5), T(0.0), T(0.001), T(0.5), T(1.0), T(5.0)
        };

        for(const T val : values)
        {
            REQUIRE(TRAP::Math::Equal(TRAP::Math::SinH(val), std::sinh(val), Epsilon));
        }
    }

    template<typename T>
    requires std::floating_point<T>
    void RunSinHEdgeTests()
    {
        static constexpr T nan = std::numeric_limits<T>::quiet_NaN();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::SinH(nan)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunSinHVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SinH(T(typename T::value_type(-0.5f))), T(-0.521095f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SinH(T(typename T::value_type(0.0f))), T(0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SinH(T(typename T::value_type(0.001f))), T(0.001f), T(0.0000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SinH(T(typename T::value_type(0.5f))), T(0.521095f), T(0.000001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SinH(T(typename T::value_type(1.0f))), T(1.1752f), T(0.00001f))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::SinH(T(typename T::value_type(5.0f))), T(74.2032f), T(0.0001f))));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunSinHVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr std::array<T, 6> values
        {
            T(typename T::value_type(-0.5)), T(typename T::value_type(0.0)), T(typename T::value_type(0.001)),
            T(typename T::value_type(0.5)), T(typename T::value_type(1.0)), T(typename T::value_type(5.0))
        };

        for(const T val : values)
        {
            const T s = TRAP::Math::SinH(val);
            for(uint32_t l = 0; l < val.Length(); ++l)
                REQUIRE(TRAP::Math::Equal(s[l], std::sinh(val[l]), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunSinHVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::SinH(T(nan)))));
    }
}

TEST_CASE("TRAP::Math::SinH()", "[math][generic][sinh]")
{
    SECTION("Scalar - double")
    {
        RunSinHRunTimeTests<double>();
        RunSinHCompileTimeTests<double>();
        RunSinHEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunSinHRunTimeTests<float>();
        RunSinHCompileTimeTests<float>();
        RunSinHEdgeTests<float>();
    }

    SECTION("Vec2 - double")
    {
        RunSinHVecRunTimeTests<TRAP::Math::Vec2d>();
        RunSinHVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunSinHVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunSinHVecRunTimeTests<TRAP::Math::Vec2f>();
        RunSinHVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunSinHVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - double")
    {
        RunSinHVecRunTimeTests<TRAP::Math::Vec3d>();
        RunSinHVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunSinHVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunSinHVecRunTimeTests<TRAP::Math::Vec3f>();
        RunSinHVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunSinHVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - double")
    {
        RunSinHVecRunTimeTests<TRAP::Math::Vec4d>();
        RunSinHVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunSinHVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunSinHVecRunTimeTests<TRAP::Math::Vec4f>();
        RunSinHVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunSinHVecEdgeTests<TRAP::Math::Vec4f>();
    }
}
