#include "TypeLength.h"

#include <TRAP.h>

namespace TypeLength
{
    int32_t TestLengthVec()
    {
        int32_t error = 0;

        error += TRAP::Math::Vec2::Length() == 2 ? 0 : 1;
        error += TRAP::Math::Vec3::Length() == 3 ? 0 : 1;
        error += TRAP::Math::Vec4::Length() == 4 ? 0 : 1;

        error += TRAP::Math::Vec2i::Length() == 2 ? 0 : 1;
        error += TRAP::Math::Vec3i::Length() == 3 ? 0 : 1;
        error += TRAP::Math::Vec4i::Length() == 4 ? 0 : 1;

        error += TRAP::Math::Vec2ui::Length() == 2 ? 0 : 1;
        error += TRAP::Math::Vec3ui::Length() == 3 ? 0 : 1;
        error += TRAP::Math::Vec4ui::Length() == 4 ? 0 : 1;

        error += TRAP::Math::Vec2d::Length() == 2 ? 0 : 1;
        error += TRAP::Math::Vec3d::Length() == 3 ? 0 : 1;
        error += TRAP::Math::Vec4d::Length() == 4 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestLengthMat()
    {
        int32_t error = 0;

        error += TRAP::Math::Mat3::Length() == 3 ? 0 : 1;
        error += TRAP::Math::Mat4::Length() == 4 ? 0 : 1;

        error += TRAP::Math::Mat3d::Length() == 3 ? 0 : 1;
        error += TRAP::Math::Mat4d::Length() == 4 ? 0 : 1;

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunTypeLengthTests()
{
    int32_t error = 0;

    error += TypeLength::TestLengthVec();
    error += TypeLength::TestLengthMat();

    return error;
}