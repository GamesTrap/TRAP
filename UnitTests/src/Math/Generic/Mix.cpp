#include <cmath>
#include <numeric>
#include <limits>
#include <concepts>

#include <catch_amalgamated.hpp>

#include "TRAP/src/Maths/Math.h"

template<typename T, typename X>
requires (std::is_arithmetic_v<T> || (TRAP::Math::IsVec<T> && std::is_arithmetic_v<typename T::value_type>)) &&
         (std::floating_point<X> || (TRAP::Math::IsVec<X> && std::same_as<typename X::value_type, bool>) || std::same_as<X, bool>)
consteval void RunCompileTimeMixTest()
{
    if constexpr(TRAP::Math::IsVec<T>)
    {
        constexpr typename T::value_type Epsilon = TRAP::Math::Epsilon<typename T::value_type>();

        constexpr T res1 = TRAP::Math::Mix(T(0), T(1), X(0.0f));
        constexpr T res2 = TRAP::Math::Mix(T(0), T(1), X(1.0f));

        if constexpr(std::signed_integral<typename T::value_type> || std::floating_point<typename T::value_type>)
        {
            constexpr T res3 = TRAP::Math::Mix(T(-1), T(1), X(0.0f));
            constexpr T res4 = TRAP::Math::Mix(T(-1), T(1), X(1.0f));

            if constexpr(T::Length() == 2)
            {
                static_assert(TRAP::Math::Equal(res1.x(), T(0).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.y(), T(0).y(), Epsilon));

                static_assert(TRAP::Math::Equal(res2.x(), T(1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.y(), T(1).y(), Epsilon));

                static_assert(TRAP::Math::Equal(res3.x(), T(-1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res3.y(), T(-1).y(), Epsilon));

                static_assert(TRAP::Math::Equal(res4.x(), T(1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res4.y(), T(1).y(), Epsilon));
            }
            else if constexpr(T::Length() == 3)
            {
                static_assert(TRAP::Math::Equal(res1.x(), T(0).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.y(), T(0).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.z(), T(0).z(), Epsilon));

                static_assert(TRAP::Math::Equal(res2.x(), T(1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.y(), T(1).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.z(), T(1).z(), Epsilon));

                static_assert(TRAP::Math::Equal(res3.x(), T(-1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res3.y(), T(-1).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res3.z(), T(-1).z(), Epsilon));

                static_assert(TRAP::Math::Equal(res4.x(), T(1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res4.y(), T(1).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res4.z(), T(1).z(), Epsilon));
            }
            else if constexpr(T::Length() == 4)
            {
                static_assert(TRAP::Math::Equal(res1.x(), T(0).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.y(), T(0).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.z(), T(0).z(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.w(), T(0).w(), Epsilon));

                static_assert(TRAP::Math::Equal(res2.x(), T(1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.y(), T(1).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.z(), T(1).z(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.w(), T(1).w(), Epsilon));

                static_assert(TRAP::Math::Equal(res3.x(), T(-1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res3.y(), T(-1).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res3.z(), T(-1).z(), Epsilon));
                static_assert(TRAP::Math::Equal(res3.w(), T(-1).w(), Epsilon));

                static_assert(TRAP::Math::Equal(res4.x(), T(1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res4.y(), T(1).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res4.z(), T(1).z(), Epsilon));
                static_assert(TRAP::Math::Equal(res4.w(), T(1).w(), Epsilon));
            }
        }
        else
        {
            if constexpr(T::Length() == 2)
            {
                static_assert(TRAP::Math::Equal(res1.x(), T(0).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.y(), T(0).y(), Epsilon));

                static_assert(TRAP::Math::Equal(res2.x(), T(1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.y(), T(1).y(), Epsilon));
            }
            else if constexpr(T::Length() == 3)
            {
                static_assert(TRAP::Math::Equal(res1.x(), T(0).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.y(), T(0).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.z(), T(0).z(), Epsilon));

                static_assert(TRAP::Math::Equal(res2.x(), T(1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.y(), T(1).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.z(), T(1).z(), Epsilon));
            }
            else if constexpr(T::Length() == 4)
            {
                static_assert(TRAP::Math::Equal(res1.x(), T(0).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.y(), T(0).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.z(), T(0).z(), Epsilon));
                static_assert(TRAP::Math::Equal(res1.w(), T(0).w(), Epsilon));

                static_assert(TRAP::Math::Equal(res2.x(), T(1).x(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.y(), T(1).y(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.z(), T(1).z(), Epsilon));
                static_assert(TRAP::Math::Equal(res2.w(), T(1).w(), Epsilon));
            }
        }
    }
    else
    {
        constexpr T Epsilon = TRAP::Math::Epsilon<T>();

        static_assert(TRAP::Math::Equal(TRAP::Math::Mix(T(0.0f), T(1.0f), X(0.0f)), T(0.0f), Epsilon));
        static_assert(TRAP::Math::Equal(TRAP::Math::Mix(T(0.0f), T(1.0f), X(1.0f)), T(1.0f), Epsilon));
        if constexpr(std::floating_point<T> || std::signed_integral<T>)
        {
            static_assert(TRAP::Math::Equal(TRAP::Math::Mix(T(-1.0f), T(1.0f), X(0.0f)), T(-1.0f), Epsilon));
            static_assert(TRAP::Math::Equal(TRAP::Math::Mix(T(-1.0f), T(1.0f), X(1.0f)), T(1.0f), Epsilon));
        }
    }
}

template<typename T>
requires std::floating_point<T>
consteval void RunCompileTimeMixEdgeTest()
{
    constexpr T Epsilon = std::numeric_limits<T>::epsilon();

    constexpr T max = std::numeric_limits<T>::max();
    constexpr T min = std::numeric_limits<T>::lowest();
    constexpr T inf = std::numeric_limits<T>::infinity();
    constexpr T ninf = -std::numeric_limits<T>::infinity();

    static_assert(TRAP::Math::Equal(TRAP::Math::Mix(min, max, T(0.0f)), min, Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::Mix(min, max, T(1.0f)), max, Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::Mix(max, max, T(0.5f)), max, Epsilon));
    static_assert(TRAP::Math::Equal(TRAP::Math::Mix(min, min, T(0.5f)), min, Epsilon));
    static_assert(TRAP::Math::IsInf(TRAP::Math::Mix(min, inf, T(1.0f))));
    static_assert(TRAP::Math::IsInf(TRAP::Math::Mix(min, ninf, T(1.0f))));
}

template<typename T>
requires TRAP::Math::IsMat<T> && std::floating_point<typename T::value_type>
consteval void RunCompileTimeMixMatTest()
{
    constexpr T A(static_cast<T::value_type>(2));
    constexpr T B(static_cast<T::value_type>(4));
    constexpr T C = TRAP::Math::Mix(A, B, 0.5);
    static_assert(TRAP::Math::All(TRAP::Math::Equal(C, T(static_cast<T::value_type>(3)), static_cast<T::value_type>(1))));
    constexpr T D = TRAP::Math::Mix(A, B, 0.5f);
    static_assert(TRAP::Math::All(TRAP::Math::Equal(D, T(static_cast<T::value_type>(3)), static_cast<T::value_type>(1))));
}

template<typename T>
requires TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>
void RunCompileTimeMixQuatTest()
{
    const T Q1(TRAP::Math::Vec<3, typename T::value_type>(1, 0, 0), TRAP::Math::Vec<3, typename T::value_type>(1, 0, 0));
    const T Q2(TRAP::Math::Vec<3, typename T::value_type>(1, 0, 0), TRAP::Math::Vec<3, typename T::value_type>(0, 1, 0));

    const T Q3 = TRAP::Math::Mix(Q1, Q2, static_cast<T::value_type>(0.5f));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Angle(Q3)), static_cast<T::value_type>(45.0f), static_cast<T::value_type>(0.001f)));

    const T Q4 = TRAP::Math::Mix(Q2, Q1, static_cast<T::value_type>(0.5f));
    REQUIRE(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Angle(Q4)), static_cast<T::value_type>(45.0f), static_cast<T::value_type>(0.001f)));

    const T A = TRAP::Math::AngleAxis(static_cast<T::value_type>(0.0f), TRAP::Math::Vec<3, typename T::value_type>(static_cast<T::value_type>(0.0f), static_cast<T::value_type>(0.0f), static_cast<T::value_type>(1.0f)));
    const T B = TRAP::Math::AngleAxis(TRAP::Math::PI<typename T::value_type>() * static_cast<T::value_type>(0.5f), TRAP::Math::Vec<3, typename T::value_type>(static_cast<T::value_type>(0.0f), static_cast<T::value_type>(0.0f), static_cast<T::value_type>(1.0f)));
    const T C = TRAP::Math::Mix(A, B, static_cast<T::value_type>(0.5f));
    const T D = TRAP::Math::AngleAxis(TRAP::Math::PI<typename T::value_type>() * static_cast<T::value_type>(0.25f), TRAP::Math::Vec<3, typename T::value_type>(static_cast<T::value_type>(0.0f), static_cast<T::value_type>(0.0f), static_cast<T::value_type>(1.0f)));

    REQUIRE(TRAP::Math::Equal(C.x(), D.x(), static_cast<T::value_type>(0.01f)));
    REQUIRE(TRAP::Math::Equal(C.y(), D.y(), static_cast<T::value_type>(0.01f)));
    REQUIRE(TRAP::Math::Equal(C.z(), D.z(), static_cast<T::value_type>(0.01f)));
    REQUIRE(TRAP::Math::Equal(C.w(), D.w(), static_cast<T::value_type>(0.01f)));
}

TEST_CASE("TRAP::Math::Mix()", "[math][generic][mix]")
{
    SECTION("Scalar - int8_t")
    {
        RunCompileTimeMixTest<int8_t, bool>();
        RunCompileTimeMixTest<int8_t, float>();
        RunCompileTimeMixTest<int8_t, double>();
    }
    SECTION("Scalar - uint8_t")
    {
        RunCompileTimeMixTest<uint8_t, bool>();
        RunCompileTimeMixTest<uint8_t, float>();
        RunCompileTimeMixTest<uint8_t, double>();
    }
    SECTION("Scalar - int16_t")
    {
        RunCompileTimeMixTest<int16_t, bool>();
        RunCompileTimeMixTest<int16_t, float>();
        RunCompileTimeMixTest<int16_t, double>();
    }
    SECTION("Scalar - uint16_t")
    {
        RunCompileTimeMixTest<uint16_t, bool>();
        RunCompileTimeMixTest<uint16_t, float>();
        RunCompileTimeMixTest<uint16_t, double>();
    }
    SECTION("Scalar - int32_t")
    {
        RunCompileTimeMixTest<int32_t, bool>();
        RunCompileTimeMixTest<int32_t, float>();
        RunCompileTimeMixTest<int32_t, double>();
    }
    SECTION("Scalar - uint32_t")
    {
        RunCompileTimeMixTest<uint32_t, bool>();
        RunCompileTimeMixTest<uint32_t, float>();
        RunCompileTimeMixTest<uint32_t, double>();
    }
    SECTION("Scalar - int64_t")
    {
        RunCompileTimeMixTest<int64_t, bool>();
        RunCompileTimeMixTest<int64_t, float>();
        RunCompileTimeMixTest<int64_t, double>();
    }
    SECTION("Scalar - uint64_t")
    {
        RunCompileTimeMixTest<uint64_t, bool>();
        RunCompileTimeMixTest<uint64_t, float>();
        RunCompileTimeMixTest<uint64_t, double>();
    }
    SECTION("Scalar - double")
    {
        RunCompileTimeMixTest<double, bool>();
        RunCompileTimeMixTest<double, double>();
        RunCompileTimeMixEdgeTest<double>();
    }
    SECTION("Scalar - float")
    {
        RunCompileTimeMixTest<float, bool>();
        RunCompileTimeMixTest<float, float>();
        RunCompileTimeMixEdgeTest<float>();
    }

    SECTION("Vec2 - int8_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2i8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2i8, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - uint8_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2ui8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2ui8, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - int16_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2i16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2i16, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - uint16_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2ui16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2ui16, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - int32_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2i32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2i32, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - uint32_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2ui32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2ui32, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - int64_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2i64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2i64, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - uint64_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2ui64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2ui64, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - double")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2d, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2d, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - float")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2f, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2f, TRAP::Math::Vec2b>();
    }

    SECTION("Vec3 - int8_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3i8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3i8, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - uint8_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3ui8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3ui8, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - int16_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3i16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3i16, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - uint16_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3ui16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3ui16, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - int32_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3i32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3i32, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - uint32_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3ui32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3ui32, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - int64_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3i64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3i64, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - uint64_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3ui64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3ui64, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - double")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3d, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3d, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - float")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4f, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4f, TRAP::Math::Vec4b>();
    }

    SECTION("Vec4 - int8_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4i8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4i8, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - uint8_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4ui8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4ui8, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - int16_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4i16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4i16, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - uint16_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4ui16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4ui16, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - int32_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4i32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4i32, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - uint32_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4ui32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4ui32, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - int64_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4i64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4i64, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - uint64_t")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4ui64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4ui64, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - double")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4d, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4d, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - float")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4f, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4f, TRAP::Math::Vec4b>();
    }

    SECTION("Mat3 - double")
    {
        RunCompileTimeMixMatTest<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - float")
    {
        RunCompileTimeMixMatTest<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - double")
    {
        RunCompileTimeMixMatTest<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - float")
    {
        RunCompileTimeMixMatTest<TRAP::Math::Mat4f>();
    }

    SECTION("Quat - double")
    {
        RunCompileTimeMixQuatTest<TRAP::Math::Quatd>();
    }
    SECTION("Quat - float")
    {
        RunCompileTimeMixQuatTest<TRAP::Math::Quatf>();
    }
}