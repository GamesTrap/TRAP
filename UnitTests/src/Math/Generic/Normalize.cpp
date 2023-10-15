#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunNormalizeVecCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        constexpr T normalize1 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(normalize1, T(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f)), T(Epsilon))));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(normalize1), typename T::value_type(1.0f), Epsilon));

        constexpr T normalize2 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::value_type>(2.0f, 0.0f, 0.0f, 0.0f)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(normalize2, T(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f)), T(Epsilon))));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(normalize2), typename T::value_type(1.0f), Epsilon));

        constexpr T normalize3 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::value_type>(-0.6f, 0.7f, -0.5f, 0.8f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(normalize3), typename T::value_type(1.0f), Epsilon));

        if constexpr(TRAP::Math::IsVec4<T>)
        {
            static_assert(TRAP::Math::All(TRAP::Math::Equal(normalize3, T(TRAP::Math::Vec<4, typename T::value_type>(-0.45485884017063427f, 0.53066861674163057f, -0.37904901841347383f, 0.60647843849879091f)), T(0.0000001f))));
        }
        else if constexpr(TRAP::Math::IsVec3<T>)
        {
            static_assert(TRAP::Math::All(TRAP::Math::Equal(normalize3, T(TRAP::Math::Vec<3, typename T::value_type>(-0.57207757317981289f, 0.66742379748924929f, -0.47673129203957787f)), T(0.0000001f))));
        }
        else if constexpr(TRAP::Math::IsVec2<T>)
        {
            static_assert(TRAP::Math::All(TRAP::Math::Equal(normalize3, T(TRAP::Math::Vec<2, typename T::value_type>(-0.65079139475254222f, 0.75925658411108998f)), T(0.0000001f))));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunNormalizeVecRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        const T normalize1 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize1, T(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f)), T(Epsilon))));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize1), typename T::value_type(1.0f), Epsilon));

        const T normalize2 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::value_type>(2.0f, 0.0f, 0.0f, 0.0f)));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize2, T(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f)), T(Epsilon))));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize2), typename T::value_type(1.0f), Epsilon));

        const T normalize3 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::value_type>(-0.6f, 0.7f, -0.5f, 0.8f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize3), typename T::value_type(1.0f), Epsilon));

        if constexpr(TRAP::Math::IsVec4<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize3, T(TRAP::Math::Vec<4, typename T::value_type>(-0.45485884017063427f, 0.53066861674163057f, -0.37904901841347383f, 0.60647843849879091f)), T(0.0000001f))));
        }
        else if constexpr(TRAP::Math::IsVec3<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize3, T(TRAP::Math::Vec<3, typename T::value_type>(-0.57207757317981289f, 0.66742379748924929f, -0.47673129203957787f)), T(0.0000001f))));
        }
        else if constexpr(TRAP::Math::IsVec2<T>)
        {
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(normalize3, T(TRAP::Math::Vec<2, typename T::value_type>(-0.65079139475254222f, 0.75925658411108998f)), T(0.0000001f))));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunNormalizeVecEdgeTests()
    {
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        const T normalize1 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize1)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize1)));

        const T normalize2 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::value_type>(inf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize2)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize2)));

        const T normalize3 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::value_type>(ninf)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize3)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize3)));

        const T normalize4 = TRAP::Math::Normalize(T(TRAP::Math::Vec<4, typename T::value_type>(nan)));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize4)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize4)));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>
    consteval void RunNormalizeQuatCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();
        constexpr typename T::value_type PI = TRAP::Math::PI<typename T::value_type>();

        {
            constexpr T a(1.0f, 0.0f, 0.0f, 0.0f);
            constexpr T n = TRAP::Math::Normalize(a);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(a, n, Epsilon)));
        }
        {
            constexpr T a(1.0f, TRAP::Math::Vec<3, typename T::value_type>(0.0f));
            constexpr T n = TRAP::Math::Normalize(a);
            static_assert(TRAP::Math::All(TRAP::Math::Equal(a, n, Epsilon)));
        }
        {
            constexpr T q = TRAP::Math::AngleAxis(PI * typename T::value_type(0.25f), TRAP::Math::Vec<3, typename T::value_type>(0.0f, 0.0f, 1.0f));
            constexpr T n = TRAP::Math::Normalize(q);
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(n), typename T::value_type(1.0f), Epsilon));
        }
        {
            constexpr T q = TRAP::Math::AngleAxis(PI * typename T::value_type(0.25f), TRAP::Math::Vec<3, typename T::value_type>(0.0f, 0.0f, 2.0f));
            constexpr T n = TRAP::Math::Normalize(q);
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(n), typename T::value_type(1.0f), Epsilon));
        }
        {
            constexpr T q = TRAP::Math::AngleAxis(PI * typename T::value_type(0.25f), TRAP::Math::Vec<3, typename T::value_type>(1.0f, 2.0f, 3.0f));
            constexpr T n = TRAP::Math::Normalize(q);
            static_assert(TRAP::Math::Equal(TRAP::Math::Length(n), typename T::value_type(1.0f), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>
    void RunNormalizeQuatRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();
        static constexpr typename T::value_type PI = TRAP::Math::PI<typename T::value_type>();

        {
            static constexpr T a(1.0f, 0.0f, 0.0f, 0.0f);
            const T n = TRAP::Math::Normalize(a);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(a, n, Epsilon)));
        }
        {
            static constexpr T a(1.0f, TRAP::Math::Vec<3, typename T::value_type>(0.0f));
            const T n = TRAP::Math::Normalize(a);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(a, n, Epsilon)));
        }
        {
            const T q = TRAP::Math::AngleAxis(PI * typename T::value_type(0.25f), TRAP::Math::Vec<3, typename T::value_type>(0.0f, 0.0f, 1.0f));
            const T n = TRAP::Math::Normalize(q);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(n), typename T::value_type(1.0f), Epsilon));
        }
        {
            const T q = TRAP::Math::AngleAxis(PI * typename T::value_type(0.25f), TRAP::Math::Vec<3, typename T::value_type>(0.0f, 0.0f, 2.0f));
            const T n = TRAP::Math::Normalize(q);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(n), typename T::value_type(1.0f), Epsilon));
        }
        {
            const T q = TRAP::Math::AngleAxis(PI * typename T::value_type(0.25f), TRAP::Math::Vec<3, typename T::value_type>(1.0f, 2.0f, 3.0f));
            const T n = TRAP::Math::Normalize(q);
            REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(n), typename T::value_type(1.0f), Epsilon));
        }
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>
    void RunNormalizeQuatEdgeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        const T normalize1 = TRAP::Math::Normalize(T(0.0f, 0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(normalize1), typename T::value_type(1.0f), Epsilon));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(T(1.0f, 0.0f, 0.0f, 0.0f), normalize1, Epsilon)));

        const T normalize2 = TRAP::Math::Normalize(T(inf, inf, inf, inf));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize2)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize2)));

        const T normalize3 = TRAP::Math::Normalize(T(ninf, ninf, ninf, ninf));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize3)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize3)));

        const T normalize4 = TRAP::Math::Normalize(T(nan, nan, nan, nan));
        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(normalize4)));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(normalize4)));
    }
}

TEST_CASE("TRAP::Math::Normalize()", "[math][generic][normalize]")
{
    SECTION("Vec4 - double")
    {
        RunNormalizeVecRunTimeTests<TRAP::Math::Vec4d>();
        RunNormalizeVecCompileTimeTests<TRAP::Math::Vec4d>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - float")
    {
        RunNormalizeVecRunTimeTests<TRAP::Math::Vec4f>();
        RunNormalizeVecCompileTimeTests<TRAP::Math::Vec4f>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Vec3 - double")
    {
        RunNormalizeVecRunTimeTests<TRAP::Math::Vec3d>();
        RunNormalizeVecCompileTimeTests<TRAP::Math::Vec3d>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - float")
    {
        RunNormalizeVecRunTimeTests<TRAP::Math::Vec3f>();
        RunNormalizeVecCompileTimeTests<TRAP::Math::Vec3f>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec2 - double")
    {
        RunNormalizeVecRunTimeTests<TRAP::Math::Vec2d>();
        RunNormalizeVecCompileTimeTests<TRAP::Math::Vec2d>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - float")
    {
        RunNormalizeVecRunTimeTests<TRAP::Math::Vec2f>();
        RunNormalizeVecCompileTimeTests<TRAP::Math::Vec2f>();
        RunNormalizeVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Quat - double")
    {
        RunNormalizeQuatRunTimeTests<TRAP::Math::Quatd>();
        RunNormalizeQuatCompileTimeTests<TRAP::Math::Quatd>();
        RunNormalizeQuatEdgeTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunNormalizeQuatRunTimeTests<TRAP::Math::Quatf>();
        RunNormalizeQuatCompileTimeTests<TRAP::Math::Quatf>();
        RunNormalizeQuatEdgeTests<TRAP::Math::Quatf>();
    }
}