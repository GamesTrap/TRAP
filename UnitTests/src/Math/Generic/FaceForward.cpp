#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    consteval void RunCompileTimeFaceForwardVecTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        {
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(-2.0f, 0.0f, 0.0f, 0.0f));
            constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 0.0f, 0.0f));
            constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            constexpr T incident(normal);
            constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 2.0f, 3.0f, 4.0f));
            constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 1.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
        {
            constexpr T normal(TRAP::Math::Vec<4, typename T::value_type>(1.0f, 0.0f, 0.0f, 0.0f));
            constexpr T incident(TRAP::Math::Vec<4, typename T::value_type>(2.0f, 0.0f, 0.0f, 0.0f));
            constexpr T reference(TRAP::Math::Vec<4, typename T::value_type>(0.0f, 0.0f, 0.0f, 0.0f));
            static_assert(TRAP::Math::All(TRAP::Math::Equal(TRAP::Math::FaceForward(normal, incident, reference), -normal, Epsilon)));
        }
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunFaceForwardVecEdgeTests()
    {
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();

        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::FaceForward(T(inf), T(ninf), T(inf)))));
        STATIC_REQUIRE(TRAP::Math::All(TRAP::Math::IsInf(TRAP::Math::FaceForward(T(ninf), T(inf), T(ninf)))));
        REQUIRE(TRAP::Math::All(TRAP::Math::IsNaN(TRAP::Math::FaceForward(T(nan), T(nan), T(nan)))));
    }
}

TEST_CASE("TRAP::Math::FaceForward()", "[math][generic][faceforward]")
{
    SECTION("Vec4 - f64")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec4d>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec4f>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Vec3 - f64")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec3d>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec3f>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec2 - f64")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec2d>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeFaceForwardVecTests<TRAP::Math::Vec2f>();
        RunFaceForwardVecEdgeTests<TRAP::Math::Vec2f>();
    }
}