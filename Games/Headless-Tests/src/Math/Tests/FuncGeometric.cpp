#include "FuncGeometric.h"

#include <TRAP.h>

namespace FuncGeometric
{
    int32_t TestLength()
    {
        int32_t error = 0;

        float length2 = TRAP::Math::Length(TRAP::Math::Vec2(1, 0));
        float length3 = TRAP::Math::Length(TRAP::Math::Vec3(1, 0, 0));
        float length4 = TRAP::Math::Length(TRAP::Math::Vec4(1, 0, 0, 0));

        error += TRAP::Math::Equal(length2, 1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        error += TRAP::Math::Equal(length3, 1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        error += TRAP::Math::Equal(length4, 1.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestDistance()
    {
        int32_t error = 0;

        float distance2 = TRAP::Math::Distance(TRAP::Math::Vec2(1, 0), TRAP::Math::Vec2(1, 0));
        float distance3 = TRAP::Math::Distance(TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Vec3(1, 0, 0));
        float distance4 = TRAP::Math::Distance(TRAP::Math::Vec4(1, 0, 0, 0), TRAP::Math::Vec4(1, 0, 0, 0));

        error += TRAP::Math::Equal(distance2, 0.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        error += TRAP::Math::Equal(distance3, 0.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        error += TRAP::Math::Equal(distance4, 0.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestDot()
    {
        int32_t error = 0;

        float dot2 = TRAP::Math::Dot(TRAP::Math::Vec2(1), TRAP::Math::Vec2(1));
        float dot3 = TRAP::Math::Dot(TRAP::Math::Vec3(1), TRAP::Math::Vec3(1));
        float dot4 = TRAP::Math::Dot(TRAP::Math::Vec4(1), TRAP::Math::Vec4(1));

        error += TRAP::Math::Equal(dot2, 2.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        error += TRAP::Math::Equal(dot3, 3.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;
        error += TRAP::Math::Equal(dot4, 4.0f, TRAP::Math::Epsilon<float>()) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestCross()
    {
        int32_t error = 0;

        TRAP::Math::Vec3 cross1 = TRAP::Math::Cross(TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Vec3(0, 1, 0));
        TRAP::Math::Vec3 cross2 = TRAP::Math::Cross(TRAP::Math::Vec3(0, 1, 0), TRAP::Math::Vec3(1, 0, 0));

        error += TRAP::Math::All(TRAP::Math::LessThan(TRAP::Math::Abs(cross1 - TRAP::Math::Vec3(0, 0,  1)), TRAP::Math::Vec3(std::numeric_limits<float>::epsilon()))) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::LessThan(TRAP::Math::Abs(cross2 - TRAP::Math::Vec3(0, 0, -1)), TRAP::Math::Vec3(std::numeric_limits<float>::epsilon()))) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestNormalize()
    {
        int32_t error = 0;

        TRAP::Math::Vec3 normalize1 = TRAP::Math::Normalize(TRAP::Math::Vec3(1, 0, 0));
        TRAP::Math::Vec3 normalize2 = TRAP::Math::Normalize(TRAP::Math::Vec3(2, 0, 0));

        [[maybe_unused]] TRAP::Math::Vec3 normalize3 = TRAP::Math::Normalize(TRAP::Math::Vec3(-0.6, 0.7, -0.5));

        TRAP::Math::Vec3 ro = TRAP::Math::Vec3(TRAP::Math::Cos(5.0f) * 3.0f, 2.0f, TRAP::Math::Sin(5.0f) * 3.0f);
        TRAP::Math::Vec3 w = TRAP::Math::Normalize(TRAP::Math::Vec3(0, -0.2f, 0) - ro);
        TRAP::Math::Vec3 u = TRAP::Math::Normalize(TRAP::Math::Cross(w, TRAP::Math::Vec3(0, 1, 0)));
        [[maybe_unused]] TRAP::Math::Vec3 v = TRAP::Math::Cross(u, w);

        error += TRAP::Math::All(TRAP::Math::LessThan(TRAP::Math::Abs(normalize1 - TRAP::Math::Vec3(1, 0, 0)), TRAP::Math::Vec3(std::numeric_limits<float>::epsilon()))) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::LessThan(TRAP::Math::Abs(normalize2 - TRAP::Math::Vec3(1, 0, 0)), TRAP::Math::Vec3(std::numeric_limits<float>::epsilon()))) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    void TestFaceForward()
    {
        TRAP::Math::Vec3 n(0.0f, 0.0f, 1.0f);
        TRAP::Math::Vec3 i(1.0f, 0.0f, 1.0f);
        TRAP::Math::Vec3 nRef(0.0f, 0.0f, 1.0f);
        [[maybe_unused]] TRAP::Math::Vec3 f = TRAP::Math::FaceForward(n, i, nRef);
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestReflect()
    {
        int32_t error = 0;

        {
            TRAP::Math::Vec2 a(1.0f, -1.0f);
            TRAP::Math::Vec2 b(0.0f, 1.0f);
            TRAP::Math::Vec2 c = TRAP::Math::Reflect(a, b);
            error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec2(1.0, 1.0), 0.0001f)) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2d a(1.0f, -1.0f);
            TRAP::Math::Vec2d b(0.0f, 1.0f);
            TRAP::Math::Vec2d c = TRAP::Math::Reflect(a, b);
            error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec2d(1.0, 1.0), 0.0001)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestRefract()
    {
        int32_t error = 0;

        {
            TRAP::Math::Vec2 a(0.0f, -1.0f);
            TRAP::Math::Vec2 b(0.0f, 1.0f);
            TRAP::Math::Vec2 c = TRAP::Math::Refract(a, b, 0.5f);
            error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec2(0.0, -1.0), 0.0001f)) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2d a(0.0f, -1.0f);
            TRAP::Math::Vec2d b(0.0f, 1.0f);
            TRAP::Math::Vec2d c = TRAP::Math::Refract(a, b, 0.5);
            error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec2d(0.0, -1.0), 0.0001)) ? 0 : 1;
        }

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunFuncGeometricTests()
{
    int32_t error = 0;

    error += FuncGeometric::TestLength();
    error += FuncGeometric::TestDistance();
    error += FuncGeometric::TestDot();
    error += FuncGeometric::TestCross();
    error += FuncGeometric::TestNormalize();
    FuncGeometric::TestFaceForward();
    error += FuncGeometric::TestReflect();
    error += FuncGeometric::TestRefract();

    return error;
}