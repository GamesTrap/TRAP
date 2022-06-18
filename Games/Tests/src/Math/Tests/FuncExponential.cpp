#include "FuncExponential.h"

#include <TRAP.h>

namespace FuncExponential
{
    int32_t TestPow()
    {
        int32_t error = 0;

        float a = TRAP::Math::Pow(2.0f, 2.0f);
        error += TRAP::Math::Equal(a, 4.0f, 0.01f) ? 0 : 1;

        TRAP::Math::Vec2 b = TRAP::Math::Pow(TRAP::Math::Vec2(2.0f), TRAP::Math::Vec2(2.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec2(4.0f), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec3 c = TRAP::Math::Pow(TRAP::Math::Vec3(2.0f), TRAP::Math::Vec3(2.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec3(4.0f), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec4 d = TRAP::Math::Pow(TRAP::Math::Vec4(2.0f), TRAP::Math::Vec4(2.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec4(4.0f), 0.01f)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestSqrt()
    {
        int32_t error = 0;

        float a = TRAP::Math::Sqrt(4.0f);
        error += TRAP::Math::Equal(a, 2.0f, 0.01f) ? 0 : 1;

        TRAP::Math::Vec2 b = TRAP::Math::Sqrt(TRAP::Math::Vec2(4.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec2(2.0f), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec3 c = TRAP::Math::Sqrt(TRAP::Math::Vec3(4.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec3(2.0f), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec4 d = TRAP::Math::Sqrt(TRAP::Math::Vec4(4.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec4(2.0f), 0.01f)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestExp()
    {
        int32_t error = 0;

        float a = TRAP::Math::Exp(1.0f);
        error += TRAP::Math::Equal(a, TRAP::Math::e<float>(), 0.01f) ? 0 : 1;

        TRAP::Math::Vec2 b = TRAP::Math::Exp(TRAP::Math::Vec2(1.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec2(TRAP::Math::e<float>()), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec3 c = TRAP::Math::Exp(TRAP::Math::Vec3(1.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec3(TRAP::Math::e<float>()), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec4 d = TRAP::Math::Exp(TRAP::Math::Vec4(1.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec4(TRAP::Math::e<float>()), 0.01f)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestLog()
    {
        int32_t error = 0;

        const float a = TRAP::Math::Log(TRAP::Math::e<float>());
        error += TRAP::Math::Equal(a, 1.0f, 0.01f) ? 0 : 1;

        const TRAP::Math::Vec2 b = TRAP::Math::Log(TRAP::Math::Vec2(TRAP::Math::e<float>()));
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec2(1.0f), 0.01f)) ? 0 : 1;

        const TRAP::Math::Vec3 c = TRAP::Math::Log(TRAP::Math::Vec3(TRAP::Math::e<float>()));
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec3(1.0f), 0.01f)) ? 0 : 1;

        const TRAP::Math::Vec4 d = TRAP::Math::Log(TRAP::Math::Vec4(TRAP::Math::e<float>()));
        error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec4(1.0f), 0.01f)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestExp2()
    {
        int32_t error = 0;

        float a = TRAP::Math::Exp2(4.0f);
        error += TRAP::Math::Equal(a, 16.0f, 0.01f) ? 0 : 1;

        TRAP::Math::Vec2 b = TRAP::Math::Exp2(TRAP::Math::Vec2(4.0f, 3.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec2(16.0f, 8.0f), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec3 c = TRAP::Math::Exp2(TRAP::Math::Vec3(4.0f, 3.0f, 2.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec3(16.0f, 8.0f, 4.0), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec4 d = TRAP::Math::Exp2(TRAP::Math::Vec4(4.0f, 3.0f, 2.0f, 1.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec4(16.0f, 8.0f, 4.0f, 2.0f), 0.01f)) ? 0 : 1;

        float f = TRAP::Math::Exp2(23.0f);
        error += TRAP::Math::Equal(f, 8388608.0f, 0.01f) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestLog2()
    {
        int32_t error = 0;

        float a = TRAP::Math::Log2(16.0f);
        error += TRAP::Math::Equal(a, 4.0f, 0.01f) ? 0 : 1;

        TRAP::Math::Vec2 b = TRAP::Math::Log2(TRAP::Math::Vec2(16.0f, 8.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec2(4.0f, 3.0f), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec3 c = TRAP::Math::Log2(TRAP::Math::Vec3(16.0f, 8.0f, 4.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec3(4.0f, 3.0f, 2.0f), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec4 d = TRAP::Math::Log2(TRAP::Math::Vec4(16.0f, 8.0f, 4.0f, 2.0f));
        error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec4(4.0f, 3.0f, 2.0f, 1.0f), 0.01f)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestInverseSqrt()
    {
        int32_t error = 0;

        float a = TRAP::Math::InverseSqrt(16.0f) * TRAP::Math::Sqrt(16.0f);
        error += TRAP::Math::Equal(a, 1.0f, 0.01f) ? 0 : 1;

        TRAP::Math::Vec2 b = TRAP::Math::InverseSqrt(TRAP::Math::Vec2(16.0f)) * TRAP::Math::Sqrt(16.0f);
        error += TRAP::Math::All(TRAP::Math::Equal(b, TRAP::Math::Vec2(1.0f), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec3 c = TRAP::Math::InverseSqrt(TRAP::Math::Vec3(16.0f)) * TRAP::Math::Sqrt(16.0f);
        error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec3(1.0f), 0.01f)) ? 0 : 1;

        TRAP::Math::Vec4 d = TRAP::Math::InverseSqrt(TRAP::Math::Vec4(16.0f)) * TRAP::Math::Sqrt(16.0f);
        error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec4(1.0f), 0.01f)) ? 0 : 1;

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunFuncExponentialTests()
{
    int32_t error = 0;

    error += FuncExponential::TestPow();
    error += FuncExponential::TestSqrt();
    error += FuncExponential::TestExp();
    error += FuncExponential::TestLog();
    error += FuncExponential::TestExp2();
    error += FuncExponential::TestLog2();
    error += FuncExponential::TestInverseSqrt();

    return error;
}