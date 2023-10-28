#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "TRAP/src/Maths/Math.h"

namespace
{
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
    consteval void RunCompileTimeMixQuatTest()
    {
        constexpr T Q1(TRAP::Math::Vec<3, typename T::value_type>(1, 0, 0), TRAP::Math::Vec<3, typename T::value_type>(1, 0, 0));
        constexpr T Q2(TRAP::Math::Vec<3, typename T::value_type>(1, 0, 0), TRAP::Math::Vec<3, typename T::value_type>(0, 1, 0));

        constexpr T Q3 = TRAP::Math::Mix(Q1, Q2, static_cast<T::value_type>(0.5f));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Angle(Q3)), static_cast<T::value_type>(45.0f), static_cast<T::value_type>(0.001f)));

        constexpr T Q4 = TRAP::Math::Mix(Q2, Q1, static_cast<T::value_type>(0.5f));
        static_assert(TRAP::Math::Equal(TRAP::Math::Degrees(TRAP::Math::Angle(Q4)), static_cast<T::value_type>(45.0f), static_cast<T::value_type>(0.001f)));

        constexpr T A = TRAP::Math::AngleAxis(static_cast<T::value_type>(0.0f), TRAP::Math::Vec<3, typename T::value_type>(static_cast<T::value_type>(0.0f), static_cast<T::value_type>(0.0f), static_cast<T::value_type>(1.0f)));
        constexpr T B = TRAP::Math::AngleAxis(TRAP::Math::PI<typename T::value_type>() * static_cast<T::value_type>(0.5f), TRAP::Math::Vec<3, typename T::value_type>(static_cast<T::value_type>(0.0f), static_cast<T::value_type>(0.0f), static_cast<T::value_type>(1.0f)));
        constexpr T C = TRAP::Math::Mix(A, B, static_cast<T::value_type>(0.5f));
        constexpr T D = TRAP::Math::AngleAxis(TRAP::Math::PI<typename T::value_type>() * static_cast<T::value_type>(0.25f), TRAP::Math::Vec<3, typename T::value_type>(static_cast<T::value_type>(0.0f), static_cast<T::value_type>(0.0f), static_cast<T::value_type>(1.0f)));

        static_assert(TRAP::Math::Equal(C.x(), D.x(), static_cast<T::value_type>(0.01f)));
        static_assert(TRAP::Math::Equal(C.y(), D.y(), static_cast<T::value_type>(0.01f)));
        static_assert(TRAP::Math::Equal(C.z(), D.z(), static_cast<T::value_type>(0.01f)));
        static_assert(TRAP::Math::Equal(C.w(), D.w(), static_cast<T::value_type>(0.01f)));
    }

    template<typename T>
    requires TRAP::Math::IsQuat<T> && std::floating_point<typename T::value_type>
    void RunRunTimeMixQuatTest()
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
}

TEST_CASE("TRAP::Math::Mix()", "[math][generic][mix]")
{
    SECTION("Scalar - i8")
    {
        RunCompileTimeMixTest<i8, bool>();
        RunCompileTimeMixTest<i8, f32>();
        RunCompileTimeMixTest<i8, f64>();
    }
    SECTION("Scalar - u8")
    {
        RunCompileTimeMixTest<u8, bool>();
        RunCompileTimeMixTest<u8, f32>();
        RunCompileTimeMixTest<u8, f64>();
    }
    SECTION("Scalar - i16")
    {
        RunCompileTimeMixTest<i16, bool>();
        RunCompileTimeMixTest<i16, f32>();
        RunCompileTimeMixTest<i16, f64>();
    }
    SECTION("Scalar - u16")
    {
        RunCompileTimeMixTest<u16, bool>();
        RunCompileTimeMixTest<u16, f32>();
        RunCompileTimeMixTest<u16, f64>();
    }
    SECTION("Scalar - i32")
    {
        RunCompileTimeMixTest<i32, bool>();
        RunCompileTimeMixTest<i32, f32>();
        RunCompileTimeMixTest<i32, f64>();
    }
    SECTION("Scalar - u32")
    {
        RunCompileTimeMixTest<u32, bool>();
        RunCompileTimeMixTest<u32, f32>();
        RunCompileTimeMixTest<u32, f64>();
    }
    SECTION("Scalar - i64")
    {
        RunCompileTimeMixTest<i64, bool>();
        RunCompileTimeMixTest<i64, f32>();
        RunCompileTimeMixTest<i64, f64>();
    }
    SECTION("Scalar - u64")
    {
        RunCompileTimeMixTest<u64, bool>();
        RunCompileTimeMixTest<u64, f32>();
        RunCompileTimeMixTest<u64, f64>();
    }
    SECTION("Scalar - f64")
    {
        RunCompileTimeMixTest<f64, bool>();
        RunCompileTimeMixTest<f64, f64>();
        RunCompileTimeMixEdgeTest<f64>();
    }
    SECTION("Scalar - f32")
    {
        RunCompileTimeMixTest<f32, bool>();
        RunCompileTimeMixTest<f32, f32>();
        RunCompileTimeMixEdgeTest<f32>();
    }

    SECTION("Vec2 - i8")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2i8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2i8, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - u8")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2ui8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2ui8, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - i16")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2i16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2i16, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - u16")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2ui16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2ui16, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - i32")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2i32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2i32, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - u32")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2ui32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2ui32, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - i64")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2i64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2i64, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - u64")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2ui64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2ui64, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - f64")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2d, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2d, TRAP::Math::Vec2b>();
    }
    SECTION("Vec2 - f32")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec2f, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec2f, TRAP::Math::Vec2b>();
    }

    SECTION("Vec3 - i8")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3i8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3i8, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - u8")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3ui8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3ui8, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - i16")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3i16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3i16, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - u16")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3ui16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3ui16, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - i32")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3i32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3i32, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - u32")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3ui32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3ui32, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - i64")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3i64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3i64, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - u64")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3ui64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3ui64, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - f64")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec3d, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec3d, TRAP::Math::Vec3b>();
    }
    SECTION("Vec3 - f32")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4f, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4f, TRAP::Math::Vec4b>();
    }

    SECTION("Vec4 - i8")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4i8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4i8, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - u8")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4ui8, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4ui8, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - i16")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4i16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4i16, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - u16")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4ui16, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4ui16, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - i32")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4i32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4i32, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - u32")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4ui32, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4ui32, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - i64")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4i64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4i64, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - u64")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4ui64, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4ui64, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - f64")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4d, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4d, TRAP::Math::Vec4b>();
    }
    SECTION("Vec4 - f32")
    {
        RunCompileTimeMixTest<TRAP::Math::Vec4f, bool>();
        RunCompileTimeMixTest<TRAP::Math::Vec4f, TRAP::Math::Vec4b>();
    }

    SECTION("Mat3 - f64")
    {
        RunCompileTimeMixMatTest<TRAP::Math::Mat3d>();
    }
    SECTION("Mat3 - f32")
    {
        RunCompileTimeMixMatTest<TRAP::Math::Mat3f>();
    }

    SECTION("Mat4 - f64")
    {
        RunCompileTimeMixMatTest<TRAP::Math::Mat4d>();
    }
    SECTION("Mat4 - f32")
    {
        RunCompileTimeMixMatTest<TRAP::Math::Mat4f>();
    }

    SECTION("Quat - f64")
    {
        RunRunTimeMixQuatTest<TRAP::Math::Quatd>();
        RunCompileTimeMixQuatTest<TRAP::Math::Quatd>();
    }
    SECTION("Quat - f32")
    {
        RunRunTimeMixQuatTest<TRAP::Math::Quatf>();
        RunCompileTimeMixQuatTest<TRAP::Math::Quatf>();
    }
}