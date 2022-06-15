#include "VectorSized.h"

#include <TRAP.h>

namespace VectorSized
{
    int32_t TestVec2Size()
    {
        int32_t error = 0;

        error += sizeof(TRAP::Math::Vec2i8) == 2 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec2i16) == 4 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec2i32) == 8 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec2i64) == 16 ? 0 : 1;

        error += sizeof(TRAP::Math::Vec2ui8) == 2 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec2ui16) == 4 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec2ui32) == 8 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec2ui64) == 16 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestVec2Comp()
    {
        int32_t error = 0;

        error += sizeof(TRAP::Math::Vec2i8) < sizeof(TRAP::Math::Vec2i16) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec2i16) < sizeof(TRAP::Math::Vec2i32) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec2i32) < sizeof(TRAP::Math::Vec2i64) ? 0 : 1;

        error += sizeof(TRAP::Math::Vec2ui8) < sizeof(TRAP::Math::Vec2ui16) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec2ui16) < sizeof(TRAP::Math::Vec2ui32) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec2ui32) < sizeof(TRAP::Math::Vec2ui64) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestVec3Size()
    {
        int32_t error = 0;

        error += sizeof(TRAP::Math::Vec3i8) == 3 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec3i16) == 6 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec3i32) == 12 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec3i64) == 24 ? 0 : 1;

        error += sizeof(TRAP::Math::Vec3ui8) == 3 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec3ui16) == 6 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec3ui32) == 12 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec3ui64) == 24 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestVec3Comp()
    {
        int32_t error = 0;

        error += sizeof(TRAP::Math::Vec3i8) < sizeof(TRAP::Math::Vec3i16) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec3i16) < sizeof(TRAP::Math::Vec3i32) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec3i32) < sizeof(TRAP::Math::Vec3i64) ? 0 : 1;

        error += sizeof(TRAP::Math::Vec3ui8) < sizeof(TRAP::Math::Vec3ui16) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec3ui16) < sizeof(TRAP::Math::Vec3ui32) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec3ui32) < sizeof(TRAP::Math::Vec3ui64) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestVec4Size()
    {
        int32_t error = 0;

        error += sizeof(TRAP::Math::Vec4i8) == 4 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec4i16) == 8 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec4i32) == 16 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec4i64) == 32 ? 0 : 1;

        error += sizeof(TRAP::Math::Vec4ui8) == 4 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec4ui16) == 8 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec4ui32) == 16 ? 0 : 1;
        error += sizeof(TRAP::Math::Vec4ui64) == 32 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestVec4Comp()
    {
        int32_t error = 0;

        error += sizeof(TRAP::Math::Vec4i8) < sizeof(TRAP::Math::Vec4i16) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec4i16) < sizeof(TRAP::Math::Vec4i32) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec4i32) < sizeof(TRAP::Math::Vec4i64) ? 0 : 1;

        error += sizeof(TRAP::Math::Vec4ui8) < sizeof(TRAP::Math::Vec4ui16) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec4ui16) < sizeof(TRAP::Math::Vec4ui32) ? 0 : 1;
        error += sizeof(TRAP::Math::Vec4ui32) < sizeof(TRAP::Math::Vec4ui64) ? 0 : 1;

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunVectorSizedTests()
{
    int32_t error = 0;

    error += VectorSized::TestVec2Size();
    error += VectorSized::TestVec2Comp();

    error += VectorSized::TestVec3Size();
    error += VectorSized::TestVec3Comp();

    error += VectorSized::TestVec4Size();
    error += VectorSized::TestVec4Comp();

    return error;
}