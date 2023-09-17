#include <cstdint>
#include <cmath>
#include <numeric>
#include <limits>
#include <array>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T>
requires std::floating_point<T>
void RunConjugateTests()
{
    static constexpr T Epsilon = TRAP::Math::Epsilon<T>();

    {
        const TRAP::Math::tQuat<T> a(TRAP::Math::tVec3<T>(1.0f, 0.0f, 0.0f), TRAP::Math::tVec3<T>(0.0f, 1.0f, 0.0));
        const TRAP::Math::tQuat<T> c = TRAP::Math::Conjugate(a);
        REQUIRE(TRAP::Math::Any(TRAP::Math::NotEqual(a, c, Epsilon)));

        const TRAP::Math::tQuat<T> b = TRAP::Math::Conjugate(c);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(a, b, Epsilon)));
    }
    {
        static constexpr TRAP::Math::tQuat<T> input(0.0f, 1.0f, 2.0f, 3.0f);
        static constexpr TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(res, TRAP::Math::tQuat<T>(0.0f, -1.0f, -2.0f, -3.0f), Epsilon)));
    }
    {
        static constexpr TRAP::Math::tQuat<T> input(2.0f, 0.0f, 0.0f, 0.0f);
        static constexpr TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(res, TRAP::Math::tQuat<T>(2.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
    }
    {
        static constexpr TRAP::Math::tQuat<T> input(1.0f, 2.0f, -3.0f, 4.0f);
        static constexpr TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(res, TRAP::Math::tQuat<T>(1.0f, -2.0f, 3.0f, -4.0f), Epsilon)));
    }
    {
        const TRAP::Math::tQuat<T> input(1.0f, 2.0f, 3.0f, 4.0f);
        const TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        REQUIRE(TRAP::Math::All(TRAP::Math::Equal(res, TRAP::Math::tQuat<T>(1.0f, -2.0f, -3.0f, -4.0f), Epsilon)));
    }
    {
        static constexpr TRAP::Math::tQuat<T> input(1.0f, 0.0f, 0.0f, 0.0f);
        static constexpr TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(res, TRAP::Math::tQuat<T>(1.0f, 0.0f, 0.0f, 0.0f), Epsilon)));
    }
    {
        static constexpr TRAP::Math::tQuat<T> input(-1.0f, -2.0f, -3.0f, -4.0f);
        static constexpr TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(res, TRAP::Math::tQuat<T>(-1.0f, 2.0f, 3.0f, 4.0f), Epsilon)));
    }
}

template<typename T>
requires std::floating_point<T>
consteval void RunCompileTimeConjugateEdgeTests()
{
    constexpr T Epsilon = TRAP::Math::Epsilon<T>();

    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T max = std::numeric_limits<T>::max();
    constexpr T nan = std::numeric_limits<T>::quiet_NaN();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    {
        constexpr TRAP::Math::tQuat<T> input(min, -1.0f, 0.0f, 1.0f);
        constexpr TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(res, TRAP::Math::tQuat<T>(min, 1.0f, 0.0f, -1.0f), Epsilon)));
    }
    {
        constexpr TRAP::Math::tQuat<T> input(max, 2.0f, -3.0f, 4.0f);
        constexpr TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        static_assert(TRAP::Math::All(TRAP::Math::Equal(res, TRAP::Math::tQuat<T>(max, -2.0f, 3.0f, -4.0f), Epsilon)));
    }
    {
        constexpr TRAP::Math::tQuat<T> input(inf, 2.0f, -3.0f, 4.0f);
        constexpr TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        static_assert(TRAP::Math::Any(TRAP::Math::IsInf(res)));
    }
    {
        constexpr TRAP::Math::tQuat<T> input(ninf, 2.0f, -3.0f, 4.0f);
        constexpr TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        static_assert(TRAP::Math::Any(TRAP::Math::IsInf(res)));
    }
    {
        constexpr TRAP::Math::tQuat<T> input(nan, 2.0f, -3.0f, 4.0f);
        constexpr TRAP::Math::tQuat<T> res = TRAP::Math::Conjugate(input);
        static_assert(TRAP::Math::Any(TRAP::Math::IsNaN(res)));
    }
}

TEST_CASE("TRAP::Math::Conjugate()", "[math][generic][conjugate]")
{
    SECTION("Scalar - double")
    {
        RunConjugateTests<double>();
        RunCompileTimeConjugateEdgeTests<double>();
    }
    SECTION("Scalar - float")
    {
        RunConjugateTests<float>();
        RunCompileTimeConjugateEdgeTests<float>();
    }
}