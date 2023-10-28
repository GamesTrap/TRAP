#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
    template<typename T>
    requires TRAP::Math::IsVec2<T> && std::floating_point<typename T::value_type>
    consteval void RunLengthVec2CompileTimeTests()
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(2.0f, 3.0f)), typename T::value_type(3.6055512754639891f), typename T::value_type(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(-5.0f, -9.0f)), typename T::value_type(10.295630140987001f), typename T::value_type(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f)), typename T::value_type(0.0f), typename T::value_type(0.000001f)));
    }

    template<typename T>
    requires TRAP::Math::IsVec2<T> && std::floating_point<typename T::value_type>
    void RunLengthVec2RunTimeTests()
    {
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(2.0f, 3.0f)), typename T::value_type(3.6055512754639891f), typename T::value_type(0.000001f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(-5.0f, -9.0f)), typename T::value_type(10.295630140987001f), typename T::value_type(0.000001f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f)), typename T::value_type(0.0f), typename T::value_type(0.000001f)));
    }

    template<typename T>
    requires TRAP::Math::IsVec3<T> && std::floating_point<typename T::value_type>
    consteval void RunLengthVec3CompileTimeTests()
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(2.0f, 3.0f, 6.0f)), typename T::value_type(7.0f), typename T::value_type(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(-5.0f, -9.0f, -12.0f)), typename T::value_type(15.811388300841896f), typename T::value_type(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f, 0.0f)), typename T::value_type(0.0f), typename T::value_type(0.000001f)));
    }

    template<typename T>
    requires TRAP::Math::IsVec3<T> && std::floating_point<typename T::value_type>
    void RunLengthVec3RunTimeTests()
    {
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(2.0f, 3.0f, 6.0f)), typename T::value_type(7.0f), typename T::value_type(0.000001f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(-5.0f, -9.0f, -12.0f)), typename T::value_type(15.811388300841896f), typename T::value_type(0.000001f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f, 0.0f)), typename T::value_type(0.0f), typename T::value_type(0.000001f)));
    }

    template<typename T>
    requires TRAP::Math::IsVec4<T> && std::floating_point<typename T::value_type>
    consteval void RunLengthVec4CompileTimeTests()
    {
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(2.0f, 3.0f, 6.0f, 8.0f)), typename T::value_type(10.63014581273465f), typename T::value_type(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(-5.0f, -9.0f, -12.0f, -15.0f)), typename T::value_type(21.794494717703369f), typename T::value_type(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f, 0.0f, 0.0f)), typename T::value_type(0.0f), typename T::value_type(0.000001f)));
    }

    template<typename T>
    requires TRAP::Math::IsVec4<T> && std::floating_point<typename T::value_type>
    void RunLengthVec4RunTimeTests()
    {
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(2.0f, 3.0f, 6.0f, 8.0f)), typename T::value_type(10.63014581273465f), typename T::value_type(0.000001f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(-5.0f, -9.0f, -12.0f, -15.0f)), typename T::value_type(21.794494717703369f), typename T::value_type(0.000001f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f, 0.0f, 0.0f)), typename T::value_type(0.0f), typename T::value_type(0.000001f)));
    }

    template<typename T>
    requires TRAP::Math::IsVec<T> && std::floating_point<typename T::value_type>
    void RunLengthVecEdgeTests()
    {
        static constexpr typename T::value_type nan = std::numeric_limits<typename T::value_type>::quiet_NaN();
        static constexpr typename T::value_type inf = std::numeric_limits<typename T::value_type>::infinity();
        static constexpr typename T::value_type ninf = -std::numeric_limits<typename T::value_type>::infinity();

        REQUIRE(TRAP::Math::IsNaN(TRAP::Math::Length(T(TRAP::Math::Vec<4, typename T::value_type>(nan, 1.0f, 2.0f, 3.0f)))));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Length(T(TRAP::Math::Vec<4, typename T::value_type>(inf, 4.0f, 6.0f, 8.0f)))));
        REQUIRE(TRAP::Math::IsInf(TRAP::Math::Length(T(TRAP::Math::Vec<4, typename T::value_type>(ninf, 4.0f, 6.0f, 8.0f)))));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    consteval void RunLengthQuatCompileTimeTests()
    {
        constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(1.0f, 2.0f, 3.0f, 4.0f)), typename T::value_type(5.4772255750516612f), typename T::value_type(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(-2.0f, -3.0f, -4.0f, -5.0f)), typename T::value_type(7.3484692283495345f), typename T::value_type(0.000001f)));
        static_assert(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f, 0.0f, 0.0f)), typename T::value_type(0.0f), Epsilon));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T>
    void RunLengthQuatRunTimeTests()
    {
        static constexpr typename T::value_type Epsilon = std::numeric_limits<typename T::value_type>::epsilon();

        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(1.0f, 2.0f, 3.0f, 4.0f)), typename T::value_type(5.4772255750516612f), typename T::value_type(0.000001f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(-2.0f, -3.0f, -4.0f, -5.0f)), typename T::value_type(7.3484692283495345f), typename T::value_type(0.000001f)));
        REQUIRE(TRAP::Math::Equal(TRAP::Math::Length(T(0.0f, 0.0f, 0.0f, 0.0f)), typename T::value_type(0.0f), Epsilon));
    }
}

TEST_CASE("TRAP::Math::Length()", "[math][generic][length]")
{
    SECTION("Vec2 - f64")
    {
        RunLengthVec2RunTimeTests<TRAP::Math::Vec2d>();
        RunLengthVec2CompileTimeTests<TRAP::Math::Vec2d>();
        RunLengthVecEdgeTests<TRAP::Math::Vec2d>();
    }
    SECTION("Vec2 - f32")
    {
        RunLengthVec2RunTimeTests<TRAP::Math::Vec2f>();
        RunLengthVec2CompileTimeTests<TRAP::Math::Vec2f>();
        RunLengthVecEdgeTests<TRAP::Math::Vec2f>();
    }

    SECTION("Vec3 - f64")
    {
        RunLengthVec3RunTimeTests<TRAP::Math::Vec3d>();
        RunLengthVec3CompileTimeTests<TRAP::Math::Vec3d>();
        RunLengthVecEdgeTests<TRAP::Math::Vec3d>();
    }
    SECTION("Vec3 - f32")
    {
        RunLengthVec3RunTimeTests<TRAP::Math::Vec3f>();
        RunLengthVec3CompileTimeTests<TRAP::Math::Vec3f>();
        RunLengthVecEdgeTests<TRAP::Math::Vec3f>();
    }

    SECTION("Vec4 - f64")
    {
        RunLengthVec4RunTimeTests<TRAP::Math::Vec4d>();
        RunLengthVec4CompileTimeTests<TRAP::Math::Vec4d>();
        RunLengthVecEdgeTests<TRAP::Math::Vec4d>();
    }
    SECTION("Vec4 - f32")
    {
        RunLengthVec4RunTimeTests<TRAP::Math::Vec4f>();
        RunLengthVec4CompileTimeTests<TRAP::Math::Vec4f>();
        RunLengthVecEdgeTests<TRAP::Math::Vec4f>();
    }

    SECTION("Quat - f64")
    {
        RunLengthQuatRunTimeTests<TRAP::Math::Quatd>();
        RunLengthQuatCompileTimeTests<TRAP::Math::Quatd>();
    }
    SECTION("Quat - f32")
    {
        RunLengthQuatRunTimeTests<TRAP::Math::Quatf>();
        RunLengthQuatCompileTimeTests<TRAP::Math::Quatf>();
    }
}