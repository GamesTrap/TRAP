#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec3<T> && std::floating_point<typename T::value_type>
    consteval void RunCompileTimeCrossVec3Tests()
    {
        constexpr typename T::value_type Epsilon = TRAP::Math::Epsilon<typename T::value_type>();

        constexpr T cross1 = TRAP::Math::Cross(T(1.0f, 0.0f, 0.0f), T(0.0f, 1.0f, 0.0f));
        constexpr T cross2 = TRAP::Math::Cross(T(0.0f, 1.0f, 0.0f), T(1.0f, 0.0f, 0.0f));
        constexpr T cross3 = TRAP::Math::Cross(T(2.0f, 3.0f, 4.0f), T(5.0f, 6.0f, 7.0f));
        constexpr T cross4 = TRAP::Math::Cross(T(0.0f, 0.0f, 0.0f), T(1.0f, 2.0f, 3.0f));
        constexpr T cross5 = TRAP::Math::Cross(T(1.0f, 0.0f, 0.0f), T(0.0f, 0.0f, 0.0f));
        constexpr T cross6 = TRAP::Math::Cross(T(1.0f, 1.0f, 1.0f), T(1.0f, 1.0f, 1.0f));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(cross1, T( 0.0f, 0.0f,  1.0f), T(Epsilon))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(cross2, T( 0.0f, 0.0f, -1.0f), T(Epsilon))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(cross3, T(-3.0f, 6.0f, -3.0f), T(Epsilon))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(cross4, T( 0.0f, 0.0f,  0.0f), T(Epsilon))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(cross5, T( 0.0f, 0.0f,  0.0f), T(Epsilon))));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(cross6, T( 0.0f, 0.0f,  0.0f), T(Epsilon))));
    }

    template<typename T>
    requires TRAP::Math::IsVec3<T> && std::floating_point<typename T::value_type>
    void RunRunTimeCrossVec3Tests()
    {
        constexpr typename T::value_type Epsilon = TRAP::Math::Epsilon<typename T::value_type>();

        T cross1 = TRAP::Math::Cross(T(1.0f, 0.0f, 0.0f), T(0.0f, 1.0f, 0.0f));
        T cross2 = TRAP::Math::Cross(T(0.0f, 1.0f, 0.0f), T(1.0f, 0.0f, 0.0f));
        T cross3 = TRAP::Math::Cross(T(2.0f, 3.0f, 4.0f), T(5.0f, 6.0f, 7.0f));
        T cross4 = TRAP::Math::Cross(T(0.0f, 0.0f, 0.0f), T(1.0f, 2.0f, 3.0f));
        T cross5 = TRAP::Math::Cross(T(1.0f, 0.0f, 0.0f), T(0.0f, 0.0f, 0.0f));
        T cross6 = TRAP::Math::Cross(T(1.0f, 1.0f, 1.0f), T(1.0f, 1.0f, 1.0f));

        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross1, T( 0.0f, 0.0f,  1.0f), T(Epsilon))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross2, T( 0.0f, 0.0f, -1.0f), T(Epsilon))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross3, T(-3.0f, 6.0f, -3.0f), T(Epsilon))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross4, T( 0.0f, 0.0f,  0.0f), T(Epsilon))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross5, T( 0.0f, 0.0f,  0.0f), T(Epsilon))));
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross6, T( 0.0f, 0.0f,  0.0f), T(Epsilon))));
    }

    template<typename T>
    requires TRAP::Math::IsVec3<T> && std::floating_point<typename T::value_type>
    void RunCrossVec3EdgeTests()
    {
        static constexpr typename T::value_type min = std::numeric_limits<typename T::value_type>::lowest();
        static constexpr typename T::value_type max = std::numeric_limits<typename T::value_type>::max();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        const T cross1 = TRAP::Math::Cross(T(min, 0.0f, 0.0f), T(0.0f, min, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(cross1)));
        const T cross2 = TRAP::Math::Cross(T(max, 0.0f, 0.0f), T(0.0f, max, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsInf(cross2)));
        const T cross3 = TRAP::Math::Cross(T(inf, 0.0f, 0.0f), T(0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(cross3)));
        const T cross4 = TRAP::Math::Cross(T(ninf, 0.0f, 0.0f), T(0.0f, 0.0f, 0.0f));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(cross4)));
        const T cross5 = TRAP::Math::Cross(T(0.0f, 0.0f, 0.0f), T(0.0f, 0.0f, nan));
        REQUIRE(TRAP::Math::Any(TRAP::Math::IsNaN(cross5)));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>
    consteval void RunCompileTimeCrossQuatTests()
    {
        constexpr typename T::value_type Epsilon = TRAP::Math::Epsilon<typename T::value_type>();

        constexpr T cross1 = TRAP::Math::Cross(T(1.0f, 0.0f, 0.0f, 0.0f), T( 1.0f, 0.0f, 0.0f, 0.0f));
        constexpr T cross2 = TRAP::Math::Cross(T(1.0f, 0.0f, 0.0f, 0.0f), T( 0.0f, 1.0f, 0.0f, 0.0f));
        constexpr T cross3 = TRAP::Math::Cross(T(1.0f, 0.0f, 0.0f, 0.0f), T(-1.0f, 0.0f, 0.0f, 0.0f));
        constexpr T cross4 = TRAP::Math::Cross(T(0.0f, 0.0f, 0.0f, 0.0f), T( 0.1f, 0.2f, 0.3f, 0.4f));

        static_assert(TRAP::Math::All(TRAP::Math::Equal(cross1, T( 1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(cross2, T( 0.0f, 1.0f, 0.0f, 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(cross3, T(-1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        static_assert(TRAP::Math::All(TRAP::Math::Equal(cross4, T( 0.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>
    void RunRunTimeCrossQuatTests()
    {
        typename T::value_type Epsilon = TRAP::Math::Epsilon<typename T::value_type>();

        {
            T q1 = T(1.0f, 0.0f, 0.0f, 0.0f);
            T q2 = T( 1.0f, 0.0f, 0.0f, 0.0f);
            T cross1 = TRAP::Math::Cross(q1, q2);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross1, T( 1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        }
        {
            T q1 = T(1.0f, 0.0f, 0.0f, 0.0f);
            T q2 = T( 0.0f, 1.0f, 0.0f, 0.0f);
            T cross2 = TRAP::Math::Cross(q1, q2);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross2, T( 0.0f, 1.0f, 0.0f, 0.0f), Epsilon)));
        }
        {
            T q1 = T(1.0f, 0.0f, 0.0f, 0.0f);
            T q2 = T(-1.0f, 0.0f, 0.0f, 0.0f);
            T cross3 = TRAP::Math::Cross(q1, q2);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross3, T(-1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        }
        {
            T q1 = T(0.0f, 0.0f, 0.0f, 0.0f);
            T q2 = T(0.1f, 0.2f, 0.3f, 0.4f);
            T cross4 = TRAP::Math::Cross(q1, q2);
            REQUIRE(TRAP::Math::All(TRAP::Math::Equal(cross4, T( 0.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
        }
    }
}

TEST_CASE("TRAP::Math::Cross()", "[math][generic][cross]")
{
    SECTION("Vec3 - f64")
    {
        RunCompileTimeCrossVec3Tests<TRAP::Math::Vec3d>();
        RunRunTimeCrossVec3Tests<TRAP::Math::Vec3d>();
        RunCrossVec3EdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeCrossVec3Tests<TRAP::Math::Vec3f>();
        RunRunTimeCrossVec3Tests<TRAP::Math::Vec3f>();
        RunCrossVec3EdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Quat - f64")
    {
        RunCompileTimeCrossQuatTests<TRAP::Math::Quatd>();
        RunRunTimeCrossQuatTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - f32")
    {
        RunCompileTimeCrossQuatTests<TRAP::Math::Quatf>();
        RunRunTimeCrossQuatTests<TRAP::Math::Quatf>();
    }
}