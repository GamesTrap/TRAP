#include "FuncCommon.h"

#include <TRAP.h>

namespace FuncCommon
{
    void TestConstexpr()
    {
        static_assert(TRAP::Math::Abs(1.0f) > 0.0f, "Math: Failed constexpr");
        [[maybe_unused]] static constexpr TRAP::Math::Vec2 a = TRAP::Math::Abs(TRAP::Math::Vec2(1.0f));
        [[maybe_unused]] static constexpr TRAP::Math::Vec3 b = TRAP::Math::Abs(TRAP::Math::Vec3(1.0f));
        [[maybe_unused]] static constexpr TRAP::Math::Vec4 c = TRAP::Math::Abs(TRAP::Math::Vec4(1.0f));
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    struct Type
    {
        T Value;
        T Return;
    };

    int32_t TestSign()
    {
        constexpr std::array<Type<int32_t>, 9> data =
        {
            {
                { std::numeric_limits<int32_t>::max(),  1 },
                { std::numeric_limits<int32_t>::min(), -1 },
                {  0,  0 },
                {  1,  1 },
                {  2,  1 },
                {  3,  1 },
                { -1, -1 },
                { -2, -1 },
                { -3, -1 }
            }
        };

        constexpr std::array<Type<TRAP::Math::Vec4i>, 7> dataVec4i =
        {
            {
                { TRAP::Math::Vec4i( 1), TRAP::Math::Vec4i( 1) },
                { TRAP::Math::Vec4i( 0), TRAP::Math::Vec4i( 0) },
                { TRAP::Math::Vec4i( 2), TRAP::Math::Vec4i( 1) },
                { TRAP::Math::Vec4i( 3), TRAP::Math::Vec4i( 1) },
                { TRAP::Math::Vec4i(-1), TRAP::Math::Vec4i(-1) },
                { TRAP::Math::Vec4i(-2), TRAP::Math::Vec4i(-1) },
                { TRAP::Math::Vec4i(-3), TRAP::Math::Vec4i(-1) },
            }
        };

        constexpr std::array<Type<TRAP::Math::Vec4>, 7> dataVec4 =
        {
            {
                { TRAP::Math::Vec4( 1), TRAP::Math::Vec4( 1) },
                { TRAP::Math::Vec4( 0), TRAP::Math::Vec4( 0) },
                { TRAP::Math::Vec4( 2), TRAP::Math::Vec4( 1) },
                { TRAP::Math::Vec4( 3), TRAP::Math::Vec4( 1) },
                { TRAP::Math::Vec4(-1), TRAP::Math::Vec4(-1) },
                { TRAP::Math::Vec4(-2), TRAP::Math::Vec4(-1) },
                { TRAP::Math::Vec4(-3), TRAP::Math::Vec4(-1) },
            }
        };

        int32_t error = 0;

        for(const auto& [val, ret] : data)
        {
            const int32_t result = TRAP::Math::Sign(val);
            error += ret == result ? 0 : 1;
        }

        for(const auto& [val, ret] : dataVec4i)
        {
            const TRAP::Math::Vec4i result = TRAP::Math::Sign(val);
            error += TRAP::Math::All(TRAP::Math::Equal(ret, result)) ? 0 : 1;
        }

        for(const auto& [val, ret] : dataVec4)
        {
            const TRAP::Math::Vec4 result = TRAP::Math::Sign(val);
            error += TRAP::Math::All(TRAP::Math::Equal(ret, result, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestFloor()
    {
        int32_t error = 0;

        {
            const float a = 1.1f;
            const float b = TRAP::Math::Floor(a);
            error += TRAP::Math::Equal(b, 1.0f, 0.0001f) ? 0 : 1;
        }

        {
            const double a = 1.1;
            const double b = TRAP::Math::Floor(a);
            error += TRAP::Math::Equal(b, 1.0, 0.0001) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec2 a(1.1f);
            const TRAP::Math::Vec2 b = TRAP::Math::Floor(a);
            error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec2(1.0), 0.0001f)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec2d a(1.1);
            const TRAP::Math::Vec2d b = TRAP::Math::Floor(a);
            error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec2d(1.0), 0.0001)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3 a(1.1f);
            const TRAP::Math::Vec3 b = TRAP::Math::Floor(a);
            error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec3(1.0), 0.0001f)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3d a(1.1);
            const TRAP::Math::Vec3d b = TRAP::Math::Floor(a);
            error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec3d(1.0), 0.0001)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 a(1.1f);
            const TRAP::Math::Vec4 b = TRAP::Math::Floor(a);
            error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec4(1.0), 0.0001f)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4d a(1.1);
            const TRAP::Math::Vec4d b = TRAP::Math::Floor(a);
            error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec4d(1.0), 0.0001)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMod()
    {
        int32_t error = 0;

        {
            const float a(1.5f);
            const float b(1.0f);
            const float c = TRAP::Math::Mod(a, b);

            error += TRAP::Math::Equal(c, 0.5f, 0.00001f) ? 0 : 1;
        }

        {
            const float a(-0.2f);
            const float b(1.0f);
            const float c = TRAP::Math::Mod(a, b);

            error += TRAP::Math::Equal(c, 0.8f, 0.00001f) ? 0 : 1;
        }

        {
            const float a(3.0);
            const float b(2.0f);
            const float c = TRAP::Math::Mod(a, b);

            error += TRAP::Math::Equal(c, 1.0f, 0.00001f) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 a(3.0);
            const float b(2.0f);
            const TRAP::Math::Vec4 c = TRAP::Math::Mod(a, b);

            error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec4(1.0f), 0.00001f)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 a(3.0);
            const TRAP::Math::Vec4 b(2.0f);
            const TRAP::Math::Vec4 c = TRAP::Math::Mod(a, b);

            error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec4(1.0f), 0.00001f)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestModf()
    {
        int32_t error = 0;

        {
            const float x(1.5f);
            float i(0.0f);
            const float a = TRAP::Math::Modf(x, i);

            error += TRAP::Math::Equal(i, 1.0f, 0.0001f) ? 0 : 1;
            error += TRAP::Math::Equal(a, 0.5f, 0.0001f) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 x(1.1f, 1.2f, 1.5f, 1.7f);
            TRAP::Math::Vec4 i(0.0f);
            const TRAP::Math::Vec4 a = TRAP::Math::Modf(x, i);

            error += TRAP::Math::Vec4i(i) == TRAP::Math::Vec4i(1) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec4(0.1f, 0.2f, 0.5f, 0.7f), 0.00001f)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4d x(1.1, 1.2, 1.5, 1.7);
            TRAP::Math::Vec4d i(0.0);
            const TRAP::Math::Vec4d a = TRAP::Math::Modf(x, i);

            error += TRAP::Math::Vec4i(i) == TRAP::Math::Vec4i(1) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec4d(0.1, 0.2, 0.5, 0.7), 0.000000001)) ? 0 : 1;
        }

        {
            const double x(1.5);
            double i(0.0);
            const double a = TRAP::Math::Modf(x, i);

            error += TRAP::Math::Equal(i, 1.0, 0.0001) ? 0 : 1;
            error += TRAP::Math::Equal(a, 0.5, 0.0001) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename T, typename B>
    struct MixEntry
    {
        T x;
        T y;
        B a;
        T Result;
    };

    constexpr std::array<MixEntry<float, bool>, 4> TestBool =
    {
        {
            {  0.0f, 1.0f, false,  0.0f },
            {  0.0f, 1.0f,  true,  1.0f },
            { -1.0f, 1.0f, false, -1.0f },
            { -1.0f, 1.0f,  true,  1.0f }
        }
    };

    constexpr std::array<MixEntry<float, float>, 4> TestFloat =
    {
        {
            {  0.0f, 1.0f, 0.0f,  0.0f },
            {  0.0f, 1.0f, 1.0f,  1.0f },
            { -1.0f, 1.0f, 0.0f, -1.0f },
            { -1.0f, 1.0f, 1.0f,  1.0f }
        }
    };

    constexpr std::array<MixEntry<TRAP::Math::Vec2, bool>, 4> TestVec2Bool =
    {
        {
            { TRAP::Math::Vec2( 0.0f), TRAP::Math::Vec2(1.0f), false, TRAP::Math::Vec2( 0.0f) },
            { TRAP::Math::Vec2( 0.0f), TRAP::Math::Vec2(1.0f),  true, TRAP::Math::Vec2( 1.0f) },
            { TRAP::Math::Vec2(-1.0f), TRAP::Math::Vec2(1.0f), false, TRAP::Math::Vec2(-1.0f) },
            { TRAP::Math::Vec2(-1.0f), TRAP::Math::Vec2(1.0f),  true, TRAP::Math::Vec2( 1.0f) }
        }
    };

    constexpr std::array<MixEntry<TRAP::Math::Vec2, TRAP::Math::Vec2b>, 5> TestVec2b =
    {
        {
            { TRAP::Math::Vec2( 0.0f), TRAP::Math::Vec2(1.0f), TRAP::Math::Vec2b(false),        TRAP::Math::Vec2( 0.0f) },
            { TRAP::Math::Vec2( 0.0f), TRAP::Math::Vec2(1.0f), TRAP::Math::Vec2b( true),        TRAP::Math::Vec2( 1.0f) },
            { TRAP::Math::Vec2(-1.0f), TRAP::Math::Vec2(1.0f), TRAP::Math::Vec2b(false),        TRAP::Math::Vec2(-1.0f) },
            { TRAP::Math::Vec2(-1.0f), TRAP::Math::Vec2(1.0f), TRAP::Math::Vec2b( true),        TRAP::Math::Vec2( 1.0f) },
            { TRAP::Math::Vec2(-1.0f), TRAP::Math::Vec2(1.0f), TRAP::Math::Vec2b( true, false), TRAP::Math::Vec2( 1.0f, -1.0f) }
        }
    };

    constexpr std::array<MixEntry<TRAP::Math::Vec3, bool>, 4> TestVec3Bool =
    {
        {
            { TRAP::Math::Vec3( 0.0f), TRAP::Math::Vec3(1.0f), false, TRAP::Math::Vec3( 0.0f) },
            { TRAP::Math::Vec3( 0.0f), TRAP::Math::Vec3(1.0f),  true, TRAP::Math::Vec3( 1.0f) },
            { TRAP::Math::Vec3(-1.0f), TRAP::Math::Vec3(1.0f), false, TRAP::Math::Vec3(-1.0f) },
            { TRAP::Math::Vec3(-1.0f), TRAP::Math::Vec3(1.0f),  true, TRAP::Math::Vec3( 1.0f) }
        }
    };

    constexpr std::array<MixEntry<TRAP::Math::Vec3, TRAP::Math::Vec3b>, 5> TestVec3b =
    {
        {
            { TRAP::Math::Vec3( 0.0f),             TRAP::Math::Vec3(1.0f),             TRAP::Math::Vec3b(false),             TRAP::Math::Vec3( 0.0f) },
            { TRAP::Math::Vec3( 0.0f),             TRAP::Math::Vec3(1.0f),             TRAP::Math::Vec3b( true),             TRAP::Math::Vec3( 1.0f) },
            { TRAP::Math::Vec3(-1.0f),             TRAP::Math::Vec3(1.0f),             TRAP::Math::Vec3b(false),             TRAP::Math::Vec3(-1.0f) },
            { TRAP::Math::Vec3(-1.0f),             TRAP::Math::Vec3(1.0f),             TRAP::Math::Vec3b( true),             TRAP::Math::Vec3( 1.0f) },
            { TRAP::Math::Vec3( 1.0f, 2.0f, 3.0f), TRAP::Math::Vec3(4.0f, 5.0f, 6.0f), TRAP::Math::Vec3b(true, false, true), TRAP::Math::Vec3( 4.0f, 2.0f, 6.0f) }
        }
    };

    constexpr std::array<MixEntry<TRAP::Math::Vec4, bool>, 4> TestVec4Bool =
    {
        {
            { TRAP::Math::Vec4( 0.0f), TRAP::Math::Vec4(1.0f), false, TRAP::Math::Vec4( 0.0f) },
            { TRAP::Math::Vec4( 0.0f), TRAP::Math::Vec4(1.0f),  true, TRAP::Math::Vec4( 1.0f) },
            { TRAP::Math::Vec4(-1.0f), TRAP::Math::Vec4(1.0f), false, TRAP::Math::Vec4(-1.0f) },
            { TRAP::Math::Vec4(-1.0f), TRAP::Math::Vec4(1.0f),  true, TRAP::Math::Vec4( 1.0f) }
        }
    };

    constexpr std::array<MixEntry<TRAP::Math::Vec4, TRAP::Math::Vec4b>, 5> TestVec4b =
    {
        {
            { TRAP::Math::Vec4( 0.0f, 0.0f, 1.0f, 1.0f), TRAP::Math::Vec4(2.0f, 2.0f, 3.0f, 3.0f), TRAP::Math::Vec4b(false, true, false, true), TRAP::Math::Vec4( 0.0f, 2.0f, 1.0f, 3.0f) },
            { TRAP::Math::Vec4( 0.0f),                   TRAP::Math::Vec4(1.0f),                   TRAP::Math::Vec4b( true),                    TRAP::Math::Vec4( 1.0f) },
            { TRAP::Math::Vec4(-1.0f),                   TRAP::Math::Vec4(1.0f),                   TRAP::Math::Vec4b(false),                    TRAP::Math::Vec4(-1.0f) },
            { TRAP::Math::Vec4(-1.0f),                   TRAP::Math::Vec4(1.0f),                   TRAP::Math::Vec4b( true),                    TRAP::Math::Vec4( 1.0f) },
            { TRAP::Math::Vec4( 1.0f, 2.0f, 3.0f, 4.0f), TRAP::Math::Vec4(5.0f, 6.0f, 7.0f, 8.0f), TRAP::Math::Vec4b(true, false, true, false), TRAP::Math::Vec4( 5.0f, 2.0f, 7.0f, 4.0f) }
        }
    };

    int32_t TestMix()
    {
        int32_t error = 0;

        //Float with bool
        {
            for(const auto& entry : TestBool)
            {
                const float result = TRAP::Math::Mix(entry.x, entry.y, entry.a);
                error += TRAP::Math::Equal(result, entry.Result, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            }
        }

        //Float with float
        {
            for(const auto& entry : TestFloat)
            {
                const float result = TRAP::Math::Mix(entry.x, entry.y, entry.a);
                error += TRAP::Math::Equal(result, entry.Result, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            }
        }

        //Vec2 with bool
        {
            for(const auto& entry : TestVec2Bool)
            {
                const TRAP::Math::Vec2 result = TRAP::Math::Mix(entry.x, entry.y, entry.a);
                error += TRAP::Math::Equal(result.x, entry.Result.x, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.y, entry.Result.y, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            }
        }

        //Vec2 with Vec2b
        {
            for(const auto& entry : TestVec2b)
            {
                const TRAP::Math::Vec2 result = TRAP::Math::Mix(entry.x, entry.y, entry.a);
                error += TRAP::Math::Equal(result.x, entry.Result.x, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.y, entry.Result.y, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            }
        }

        //Vec3 with bool
        {
            for(const auto& entry : TestVec3Bool)
            {
                const TRAP::Math::Vec3 result = TRAP::Math::Mix(entry.x, entry.y, entry.a);
                error += TRAP::Math::Equal(result.x, entry.Result.x, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.y, entry.Result.y, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.z, entry.Result.z, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            }
        }

        //Vec3 with Vec3b
        {
            for(const auto& entry : TestVec3b)
            {
                const TRAP::Math::Vec3 result = TRAP::Math::Mix(entry.x, entry.y, entry.a);
                error += TRAP::Math::Equal(result.x, entry.Result.x, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.y, entry.Result.y, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.z, entry.Result.z, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            }
        }

        //Vec4 with bool
        {
            for(const auto& entry : TestVec4Bool)
            {
                const TRAP::Math::Vec4 result = TRAP::Math::Mix(entry.x, entry.y, entry.a);
                error += TRAP::Math::Equal(result.x, entry.Result.x, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.y, entry.Result.y, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.z, entry.Result.z, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.w, entry.Result.w, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            }
        }

        //Vec4 with Vec4b
        {
            for(const auto& entry : TestVec4b)
            {
                const TRAP::Math::Vec4 result = TRAP::Math::Mix(entry.x, entry.y, entry.a);
                error += TRAP::Math::Equal(result.x, entry.Result.x, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.y, entry.Result.y, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.z, entry.Result.z, TRAP::Math::Epsilon<float>()) ? 0 : 1;
                error += TRAP::Math::Equal(result.w, entry.Result.w, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            }
        }

        //Mat4
        {
            const TRAP::Math::Mat4 a(2);
            const TRAP::Math::Mat4 b(4);
            const TRAP::Math::Mat4 c = TRAP::Math::Mix(a, b, 0.5f);
            const TRAP::Math::Vec4b d = TRAP::Math::Equal(c, TRAP::Math::Mat4(3), TRAP::Math::Epsilon<float>());
            error += TRAP::Math::All(d) ? 0 : 1;
        }

        {
            const TRAP::Math::Mat4 a(2);
            const TRAP::Math::Mat4 b(4);
            const TRAP::Math::Mat4 c = TRAP::Math::Mix(a, b, 0.5);
            const TRAP::Math::Vec4b d = TRAP::Math::Equal(c, TRAP::Math::Mat4(3), TRAP::Math::Epsilon<float>());
            error += TRAP::Math::All(d) ? 0 : 1;
        }

        {
            const TRAP::Math::Mat4d a(2);
            const TRAP::Math::Mat4d b(4);
            const TRAP::Math::Mat4d c = TRAP::Math::Mix(a, b, 0.5);
            const TRAP::Math::Vec4b d = TRAP::Math::Equal(c, TRAP::Math::Mat4d(3), TRAP::Math::Epsilon<double>());
            error += TRAP::Math::All(d) ? 0 : 1;
        }

        {
            const TRAP::Math::Mat4d a(2);
            const TRAP::Math::Mat4d b(4);
            const TRAP::Math::Mat4d c = TRAP::Math::Mix(a, b, 0.5f);
            const TRAP::Math::Vec4b d = TRAP::Math::Equal(c, TRAP::Math::Mat4d(3), TRAP::Math::Epsilon<double>());
            error += TRAP::Math::All(d) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename EDGE, typename VEC>
    struct StepEntry
    {
        EDGE edge;
        VEC x;
        VEC result;
    };

    constexpr std::array<StepEntry<float, TRAP::Math::Vec4>, 3> TestVec4Scalar =
    {
        {
            { 1.0f, TRAP::Math::Vec4( 1.0f,  2.0f,  3.0f,  4.0f), TRAP::Math::Vec4(1.0f) },
            { 0.0f, TRAP::Math::Vec4( 1.0f,  2.0f,  3.0f,  4.0f), TRAP::Math::Vec4(1.0f) },
            { 0.0f, TRAP::Math::Vec4(-1.0f, -2.0f, -3.0f, -4.0f), TRAP::Math::Vec4(0.0f) }
        }
    };

    constexpr std::array<StepEntry<TRAP::Math::Vec4, TRAP::Math::Vec4>, 4> TestVec4Vector =
    {
        {
            { TRAP::Math::Vec4(-1.0f, -2.0f, -3.0f, -4.0f), TRAP::Math::Vec4(-2.0f, -3.0f, -4.0f, -5.0f), TRAP::Math::Vec4(0.0f) },
            { TRAP::Math::Vec4( 0.0f,  1.0f,  2.0f,  3.0f), TRAP::Math::Vec4( 1.0f,  2.0f,  3.0f,  4.0f), TRAP::Math::Vec4(1.0f) },
            { TRAP::Math::Vec4( 2.0f,  3.0f,  4.0f,  5.0f), TRAP::Math::Vec4( 1.0f,  2.0f,  3.0f,  4.0f), TRAP::Math::Vec4(0.0f) },
            { TRAP::Math::Vec4( 0.0f,  1.0f,  2.0f,  3.0f), TRAP::Math::Vec4(-1.0f, -2.0f, -3.0f, -4.0f), TRAP::Math::Vec4(0.0f) }
        }
    };

    int32_t TestStep()
    {
        int32_t error = 0;

        //Scalar
        {
            const float edge = 2.0f;

            const float a = TRAP::Math::Step(edge, 1.0f);
            error += TRAP::Math::Equal(a, 0.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;

            const float b = TRAP::Math::Step(edge, 3.0f);
            error += TRAP::Math::Equal(b, 1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;

            const float c = TRAP::Math::Step(edge, 2.0f);
            error += TRAP::Math::Equal(c, 1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        }

        //Vec4 and float
        {
            for(const auto& entry : TestVec4Scalar)
            {
                const TRAP::Math::Vec4 result = TRAP::Math::Step(entry.edge, entry.x);
                error += TRAP::Math::All(TRAP::Math::Equal(result, entry.result, TRAP::Math::Epsilon<float>())) ? 0 : 1;
            }
        }

        //Vec4 and Vec4
        {
            for(const auto& entry : TestVec4Vector)
            {
                const TRAP::Math::Vec4 result = TRAP::Math::Step(entry.edge, entry.x);
                error += TRAP::Math::All(TRAP::Math::Equal(result, entry.result, TRAP::Math::Epsilon<float>())) ? 0 : 1;
            }
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMax()
    {
        int32_t error = 0;

        const float a0 = TRAP::Math::Max(1.0f, 1.0f);
        const bool a1 = TRAP::Math::Equal(a0, 1.0f, TRAP::Math::Epsilon<float>());
        error += a1 ? 0 : 1;

        const TRAP::Math::Vec2 b0 = TRAP::Math::Max(TRAP::Math::Vec2(1), TRAP::Math::Vec2(1));
        const TRAP::Math::Vec2 b1 = TRAP::Math::Max(TRAP::Math::Vec2(1), 1.0f);
        const bool b2 = TRAP::Math::All(TRAP::Math::Equal(b0, b1, TRAP::Math::Epsilon<float>()));
        error += b2 ? 0 : 1;

        const TRAP::Math::Vec3 c0 = TRAP::Math::Max(TRAP::Math::Vec3(1), TRAP::Math::Vec3(1));
        const TRAP::Math::Vec3 c1 = TRAP::Math::Max(TRAP::Math::Vec3(1), 1.0f);
        const bool c2 = TRAP::Math::All(TRAP::Math::Equal(c0, c1, TRAP::Math::Epsilon<float>()));
        error += c2 ? 0 : 1;

        const TRAP::Math::Vec4 d0 = TRAP::Math::Max(TRAP::Math::Vec4(1), TRAP::Math::Vec4(1));
        const TRAP::Math::Vec4 d1 = TRAP::Math::Max(TRAP::Math::Vec4(1), 1.0f);
        const bool d2 = TRAP::Math::All(TRAP::Math::Equal(d0, d1, TRAP::Math::Epsilon<float>()));
        error += d2 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMin()
    {
        int32_t error = 0;

        const float a0 = TRAP::Math::Min(1.0f, 1.0f);
        const bool a1 = TRAP::Math::Equal(a0, 1.0f, TRAP::Math::Epsilon<float>());
        error += a1 ? 0 : 1;

        const TRAP::Math::Vec2 b0 = TRAP::Math::Min(TRAP::Math::Vec2(1), TRAP::Math::Vec2(1));
        const TRAP::Math::Vec2 b1 = TRAP::Math::Min(TRAP::Math::Vec2(1), 1.0f);
        const bool b2 = TRAP::Math::All(TRAP::Math::Equal(b0, b1, TRAP::Math::Epsilon<float>()));
        error += b2 ? 0 : 1;

        const TRAP::Math::Vec3 c0 = TRAP::Math::Min(TRAP::Math::Vec3(1), TRAP::Math::Vec3(1));
        const TRAP::Math::Vec3 c1 = TRAP::Math::Min(TRAP::Math::Vec3(1), 1.0f);
        const bool c2 = TRAP::Math::All(TRAP::Math::Equal(c0, c1, TRAP::Math::Epsilon<float>()));
        error += c2 ? 0 : 1;

        const TRAP::Math::Vec4 d0 = TRAP::Math::Min(TRAP::Math::Vec4(1), TRAP::Math::Vec4(1));
        const TRAP::Math::Vec4 d1 = TRAP::Math::Min(TRAP::Math::Vec4(1), 1.0f);
        const bool d2 = TRAP::Math::All(TRAP::Math::Equal(d0, d1, TRAP::Math::Epsilon<float>()));
        error += d2 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestRound()
    {
        int32_t error = 0;

        {
            const float a = TRAP::Math::Round(0.0f);
            error += TRAP::Math::Equal(a, 0.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float b = TRAP::Math::Round(0.5f);
            error += TRAP::Math::Equal(b, 1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float c = TRAP::Math::Round(1.0f);
            error += TRAP::Math::Equal(c, 1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float d = TRAP::Math::Round(0.1f);
            error += TRAP::Math::Equal(d, 0.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float e = TRAP::Math::Round(0.9f);
            error += TRAP::Math::Equal(e, 1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float f = TRAP::Math::Round(1.5f);
            error += TRAP::Math::Equal(f, 2.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float g = TRAP::Math::Round(1.9f);
            error += TRAP::Math::Equal(g, 2.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        }

        {
            const float a = TRAP::Math::Round(-0.0f);
            error += TRAP::Math::Equal(a, 0.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float b = TRAP::Math::Round(-0.5f);
            error += TRAP::Math::Equal(b, -1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float c = TRAP::Math::Round(-1.0f);
            error += TRAP::Math::Equal(c, -1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float d = TRAP::Math::Round(-0.1f);
            error += TRAP::Math::Equal(d, 0.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float e = TRAP::Math::Round(-0.9f);
            error += TRAP::Math::Equal(e, -1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float f = TRAP::Math::Round(-1.5f);
            error += TRAP::Math::Equal(f, -2.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float g = TRAP::Math::Round(-1.9f);
            error += TRAP::Math::Equal(g, -2.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestRoundEven()
    {
        int32_t error = 0;

        {
            const float a = TRAP::Math::RoundEven(-1.5f);
            error += TRAP::Math::Equal(a, -2.0f, 0.0001f) ? 0 : 1;
            const float b = TRAP::Math::RoundEven(1.5f);
            error += TRAP::Math::Equal(b, 2.0f, 0.0001f) ? 0 : 1;
            const float c = TRAP::Math::RoundEven(-2.5f);
            error += TRAP::Math::Equal(c, -2.0f, 0.0001f) ? 0 : 1;
            const float d = TRAP::Math::RoundEven(2.5f);
            error += TRAP::Math::Equal(d, 2.0f, 0.0001f) ? 0 : 1;
            const float e = TRAP::Math::RoundEven(-3.5f);
            error += TRAP::Math::Equal(e, -4.0f, 0.0001f) ? 0 : 1;
            const float f = TRAP::Math::RoundEven(3.5f);
            error += TRAP::Math::Equal(f, 4.0f, 0.0001f) ? 0 : 1;

            const float g = TRAP::Math::RoundEven(-4.5f);
            error += TRAP::Math::Equal(g, -4.0f, 0.0001f) ? 0 : 1;
            const float h = TRAP::Math::RoundEven(4.5f);
            error += TRAP::Math::Equal(h, 4.0f, 0.0001f) ? 0 : 1;
            const float i = TRAP::Math::RoundEven(-5.5f);
            error += TRAP::Math::Equal(i, -6.0f, 0.0001f) ? 0 : 1;
            const float j = TRAP::Math::RoundEven(5.5f);
            error += TRAP::Math::Equal(j, 6.0f, 0.0001f) ? 0 : 1;
            const float k = TRAP::Math::RoundEven(-6.5f);
            error += TRAP::Math::Equal(k, -6.0f, 0.0001f) ? 0 : 1;
            const float l = TRAP::Math::RoundEven(6.5f);
            error += TRAP::Math::Equal(l, 6.0f, 0.0001f) ? 0 : 1;
            const float m = TRAP::Math::RoundEven(-7.5f);
            error += TRAP::Math::Equal(m, -8.0f, 0.0001f) ? 0 : 1;
            const float n = TRAP::Math::RoundEven(7.5f);
            error += TRAP::Math::Equal(n, 8.0f, 0.0001f) ? 0 : 1;
        }

        {
            const float a = TRAP::Math::RoundEven(-2.4f);
            error += TRAP::Math::Equal(a, -2.0f, 0.0001f) ? 0 : 1;
            const float b = TRAP::Math::RoundEven(2.4f);
            error += TRAP::Math::Equal(b, 2.0f, 0.0001f) ? 0 : 1;
            const float c = TRAP::Math::RoundEven(-2.6f);
            error += TRAP::Math::Equal(c, -3.0f, 0.0001f) ? 0 : 1;
            const float d = TRAP::Math::RoundEven(2.6f);
            error += TRAP::Math::Equal(d, 3.0f, 0.0001f) ? 0 : 1;
            const float e = TRAP::Math::RoundEven(-2.0f);
            error += TRAP::Math::Equal(e, -2.0f, 0.0001f) ? 0 : 1;
            const float f = TRAP::Math::RoundEven(2.0f);
            error += TRAP::Math::Equal(f, 2.0f, 0.0001f) ? 0 : 1;
        }

        {
            const float a = TRAP::Math::RoundEven(0.0f);
            error += TRAP::Math::Equal(a, 0.0f, 0.0001f) ? 0 : 1;
            const float b = TRAP::Math::RoundEven(0.5f);
            error += TRAP::Math::Equal(b, 0.0f, 0.0001f) ? 0 : 1;
            const float c = TRAP::Math::RoundEven(1.0f);
            error += TRAP::Math::Equal(c, 1.0f, 0.0001f) ? 0 : 1;
            const float d = TRAP::Math::RoundEven(0.1f);
            error += TRAP::Math::Equal(d, 0.0f, 0.0001f) ? 0 : 1;
            const float e = TRAP::Math::RoundEven(0.9f);
            error += TRAP::Math::Equal(e, 1.0f, 0.0001f) ? 0 : 1;
            const float f = TRAP::Math::RoundEven(1.5f);
            error += TRAP::Math::Equal(f, 2.0f, 0.0001f) ? 0 : 1;
            const float g = TRAP::Math::RoundEven(1.9f);
            error += TRAP::Math::Equal(g, 2.0f, 0.0001f) ? 0 : 1;
        }

        {
            const float a = TRAP::Math::RoundEven(-0.0f);
            error += TRAP::Math::Equal(a, 0.0f, 0.0001f) ? 0 : 1;
            const float b = TRAP::Math::RoundEven(-0.5f);
            error += TRAP::Math::Equal(b, -0.0f, 0.0001f) ? 0 : 1;
            const float c = TRAP::Math::RoundEven(-1.0f);
            error += TRAP::Math::Equal(c, -1.0f, 0.0001f) ? 0 : 1;
            const float d = TRAP::Math::RoundEven(-0.1f);
            error += TRAP::Math::Equal(d, 0.0f, 0.0001f) ? 0 : 1;
            const float e = TRAP::Math::RoundEven(-0.9f);
            error += TRAP::Math::Equal(e, -1.0f, 0.0001f) ? 0 : 1;
            const float f = TRAP::Math::RoundEven(-1.5f);
            error += TRAP::Math::Equal(f, -2.0f, 0.0001f) ? 0 : 1;
            const float g = TRAP::Math::RoundEven(-1.9f);
            error += TRAP::Math::Equal(g, -2.0f, 0.0001f) ? 0 : 1;
        }

        {
            const float a = TRAP::Math::RoundEven(1.5f);
            error += TRAP::Math::Equal(a, 2.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float b = TRAP::Math::RoundEven(2.5f);
            error += TRAP::Math::Equal(b, 2.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float c = TRAP::Math::RoundEven(3.5f);
            error += TRAP::Math::Equal(c, 4.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float d = TRAP::Math::RoundEven(4.5f);
            error += TRAP::Math::Equal(d, 4.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float e = TRAP::Math::RoundEven(5.5f);
            error += TRAP::Math::Equal(e, 6.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float f = TRAP::Math::RoundEven(6.5f);
            error += TRAP::Math::Equal(f, 6.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float g = TRAP::Math::RoundEven(7.5f);
            error += TRAP::Math::Equal(g, 8.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        }

        {
            const float a = TRAP::Math::RoundEven(-1.5f);
            error += TRAP::Math::Equal(a, -2.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float b = TRAP::Math::RoundEven(-2.5f);
            error += TRAP::Math::Equal(b, -2.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float c = TRAP::Math::RoundEven(-3.5f);
            error += TRAP::Math::Equal(c, -4.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float d = TRAP::Math::RoundEven(-4.5f);
            error += TRAP::Math::Equal(d, -4.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float e = TRAP::Math::RoundEven(-5.5f);
            error += TRAP::Math::Equal(e, -6.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float f = TRAP::Math::RoundEven(-6.5f);
            error += TRAP::Math::Equal(f, -6.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            const float g = TRAP::Math::RoundEven(-7.5f);
            error += TRAP::Math::Equal(g, -8.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestIsNaN()
    {
        int32_t error = 0;

        constexpr float zeroF = 0.0;
        constexpr double zeroD = 0.0;

        {
            error += true == TRAP::Math::IsNaN(0.0 / zeroD) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Vec2d(0.0 / zeroD))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Vec3d(0.0 / zeroD))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Vec4d(0.0 / zeroD))) ? 0 : 1;
        }

        {
            error += true == TRAP::Math::IsNaN(0.0f / zeroF) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Vec2(0.0f / zeroF))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Vec3(0.0f / zeroF))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsNaN(TRAP::Math::Vec4(0.0f / zeroF))) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestIsInf()
    {
        int32_t error = 0;

        constexpr float zeroF = 0.0;
        constexpr double zeroD = 0.0;

        {
            error += true == TRAP::Math::IsInf( 1.0 / zeroD) ? 0 : 1;
            error += true == TRAP::Math::IsInf(-1.0 / zeroD) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec2d( 1.0 / zeroD))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec2d(-1.0 / zeroD))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec3d( 1.0 / zeroD))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec3d(-1.0 / zeroD))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec4d( 1.0 / zeroD))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec4d(-1.0 / zeroD))) ? 0 : 1;
        }

        {
            error += true == TRAP::Math::IsInf( 1.0f / zeroF) ? 0 : 1;
            error += true == TRAP::Math::IsInf(-1.0f / zeroF) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec2d( 1.0f / zeroF))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec2d(-1.0f / zeroF))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec3d( 1.0f / zeroF))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec3d(-1.0f / zeroF))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec4d( 1.0f / zeroF))) ? 0 : 1;
            error += true == TRAP::Math::Any(TRAP::Math::IsInf(TRAP::Math::Vec4d(-1.0f / zeroF))) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestFrExp()
    {
        int32_t error = 0;

        {
            const float x(1024);
            int32_t exp;
            const float a = TRAP::Math::FrExp(x, exp);
            error += TRAP::Math::Equal(a, 0.5f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
            error += TRAP::Math::Equal(exp, 11) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec2 x(1024, 0.24);
            TRAP::Math::Vec2i exp;
            const TRAP::Math::Vec2 a = TRAP::Math::FrExp(x, exp);
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec2(0.5, 0.96), TRAP::Math::Epsilon<float>())) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(exp, TRAP::Math::Vec2i(11, -2))) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3 x(1024, 0.24, 0);
            TRAP::Math::Vec3i exp;
            const TRAP::Math::Vec3 a = TRAP::Math::FrExp(x, exp);
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec3(0.5, 0.96, 0.0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(exp, TRAP::Math::Vec3i(11, -2, 0))) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 x(1024, 0.24, 0, -1.33);
            TRAP::Math::Vec4i exp;
            const TRAP::Math::Vec4 a = TRAP::Math::FrExp(x, exp);
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec4(0.5, 0.96, 0.0, -0.665), TRAP::Math::Epsilon<float>())) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(exp, TRAP::Math::Vec4i(11, -2, 0, 1))) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestLdExp()
    {
        int32_t error = 0;

        {
            const float a = 0.5f;
            int32_t exp = 11;
            const float x = TRAP::Math::LdExp(a, exp);
            error += TRAP::Math::Equal(x, 1024.0f, 0.00001f) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec2 a = TRAP::Math::Vec2(0.5, 0.96);
            TRAP::Math::Vec2i exp = TRAP::Math::Vec2i(11, -2);
            const TRAP::Math::Vec2 x = TRAP::Math::LdExp(a, exp);
            error += TRAP::Math::All(TRAP::Math::Equal(x, TRAP::Math::Vec2(1024, 0.24), 0.00001f)) ? 0 : 1;
        }


        {
            const TRAP::Math::Vec3 a = TRAP::Math::Vec3(0.5, 0.96, 0.0);
            TRAP::Math::Vec3i exp = TRAP::Math::Vec3i(11, -2, 0);
            const TRAP::Math::Vec3 x = TRAP::Math::LdExp(a, exp);
            error += TRAP::Math::All(TRAP::Math::Equal(x, TRAP::Math::Vec3(1024, 0.24, 0), 0.00001f)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 a = TRAP::Math::Vec4(0.5, 0.96, 0.0, -0.665);
            TRAP::Math::Vec4i exp = TRAP::Math::Vec4i(11, -2, 0, 1);
            const TRAP::Math::Vec4 x = TRAP::Math::LdExp(a, exp);
            error += TRAP::Math::All(TRAP::Math::Equal(x, TRAP::Math::Vec4(1024, 0.24, 0, -1.33), 0.00001f)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestFMod()
    {
        int32_t error = 0;

        {
            const float a0(3.0);
            const float b0(2.0f);
            const float c0 = TRAP::Math::FMod(a0, b0);

            error += TRAP::Math::Abs(c0 - 1.0f) < 0.00001f ? 0 : 1;

            const TRAP::Math::Vec4 a1(3.0);
            const float b1(2.0f);
            const TRAP::Math::Vec4 c1 = TRAP::Math::FMod(a1, b1);

            error += TRAP::Math::All(TRAP::Math::Equal(c1, TRAP::Math::Vec4(1.0f), 0.00001f)) ? 0 : 1;

            const TRAP::Math::Vec4 a2(3.0);
            const TRAP::Math::Vec4 b2(2.0f);
            const TRAP::Math::Vec4 c2 = TRAP::Math::FMod(a2, b2);

            error += TRAP::Math::All(TRAP::Math::Equal(c2, TRAP::Math::Vec4(1.0f), 0.00001f)) ? 0 : 1;

            const TRAP::Math::Vec4i a3(3);
            const int32_t b3(2);
            const TRAP::Math::Vec4i c3 = TRAP::Math::FMod(a3, b3);

            error += TRAP::Math::All(TRAP::Math::Equal(c3, TRAP::Math::Vec4i(1))) ? 0 : 1;

            const TRAP::Math::Vec4i a4(3);
            const TRAP::Math::Vec4i b4(2);
            const TRAP::Math::Vec4i c4 = TRAP::Math::FMod(a4, b4);

            error += TRAP::Math::All(TRAP::Math::Equal(c4, TRAP::Math::Vec4i(1))) ? 0 : 1;
        }

        {
            const float a0(22.0);
            const float b0(-10.0f);
            const float c0 = TRAP::Math::FMod(a0, b0);

            error += TRAP::Math::Abs(c0 - 2.0f) < 0.00001f ? 0 : 1;

            const TRAP::Math::Vec4 a1(22.0);
            const float b1(-10.0f);
            const TRAP::Math::Vec4 c1 = TRAP::Math::FMod(a1, b1);

            error += TRAP::Math::All(TRAP::Math::Equal(c1, TRAP::Math::Vec4(2.0f), 0.00001f)) ? 0 : 1;

            const TRAP::Math::Vec4 a2(22.0);
            const TRAP::Math::Vec4 b2(-10.0f);
            const TRAP::Math::Vec4 c2 = TRAP::Math::FMod(a2, b2);

            error += TRAP::Math::All(TRAP::Math::Equal(c2, TRAP::Math::Vec4(2.0f), 0.00001f)) ? 0 : 1;

            const TRAP::Math::Vec4i a3(22);
            const int32_t b3(-10);
            const TRAP::Math::Vec4i c3 = TRAP::Math::FMod(a3, b3);

            error += TRAP::Math::All(TRAP::Math::Equal(c3, TRAP::Math::Vec4i(2))) ? 0 : 1;

            const TRAP::Math::Vec4i a4(22);
            const TRAP::Math::Vec4i b4(-10);
            const TRAP::Math::Vec4i c4 = TRAP::Math::FMod(a4, b4);

            error += TRAP::Math::All(TRAP::Math::Equal(c4, TRAP::Math::Vec4i(2))) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestIsFinite()
    {
        int32_t error = 0;

        error += TRAP::Math::IsFinite(1.0f) ? 0 : 1;
        error += TRAP::Math::IsFinite(1.0) ? 0 : 1;
        error += TRAP::Math::IsFinite(-1.0f) ? 0 : 1;
        error += TRAP::Math::IsFinite(-1.0) ? 0 : 1;

        error += TRAP::Math::All(TRAP::Math::IsFinite(TRAP::Math::Vec4(1.0f))) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::IsFinite(TRAP::Math::Vec4d(1.0))) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::IsFinite(TRAP::Math::Vec4(-1.0f))) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::IsFinite(TRAP::Math::Vec4d(-1.0))) ? 0 : 1;

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunFuncCommonTests()
{
    int32_t error = 0;

    FuncCommon::TestConstexpr();
    error += FuncCommon::TestSign();
    error += FuncCommon::TestFloor();
    error += FuncCommon::TestMod();
    error += FuncCommon::TestModf();
    error += FuncCommon::TestMix();
    error += FuncCommon::TestStep();
    error += FuncCommon::TestMax();
    error += FuncCommon::TestMin();
    error += FuncCommon::TestRound();
    error += FuncCommon::TestRoundEven();
    error += FuncCommon::TestIsNaN();
    error += FuncCommon::TestIsInf();
    error += FuncCommon::TestFrExp();
    error += FuncCommon::TestLdExp();
    error += FuncCommon::TestFMod();
    error += FuncCommon::TestIsFinite();

    return error;
}