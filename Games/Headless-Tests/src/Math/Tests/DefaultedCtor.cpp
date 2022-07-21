#include "DefaultedCtor.h"

#include <TRAP.h>

namespace DefaultedCtor
{
    int32_t TestVecMemcpy()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Vec2i a = TRAP::Math::Vec2i(76);
            TRAP::Math::Vec2i b;
            memcpy(&b, &a, sizeof(TRAP::Math::Vec2i));
            error += b == a ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3i a = TRAP::Math::Vec3i(76);
            TRAP::Math::Vec3i b;
            memcpy(&b, &a, sizeof(TRAP::Math::Vec3i));
            error += b == a ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4i a = TRAP::Math::Vec4i(76);
            TRAP::Math::Vec4i b;
            memcpy(&b, &a, sizeof(TRAP::Math::Vec4i));
            error += b == a ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMatMemcpy()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Mat3 a = TRAP::Math::Mat3(76);
            TRAP::Math::Mat3 b;
            memcpy(&b, &a, sizeof(TRAP::Math::Mat3));
            error += TRAP::Math::All(TRAP::Math::Equal(b, a, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            const TRAP::Math::Mat4 a = TRAP::Math::Mat4(76);
            TRAP::Math::Mat4 b;
            memcpy(&b, &a, sizeof(TRAP::Math::Mat4));
            error += TRAP::Math::All(TRAP::Math::Equal(b, a, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestQuatMemcpy()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Quat a = TRAP::Math::Quat(1, 0, 0, 0);
            TRAP::Math::Quat b;
            memcpy(&b, &a, sizeof(TRAP::Math::Quat));
            error += TRAP::Math::All(TRAP::Math::Equal(b, a, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunDefaultedCtorTests()
{
    int32_t error = 0;

    error += DefaultedCtor::TestVecMemcpy();
    error += DefaultedCtor::TestMatMemcpy();
    error += DefaultedCtor::TestQuatMemcpy();

    return error;
}