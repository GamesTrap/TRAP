#include "FuncVectorRelational.h"

#include <TRAP.h>

namespace FuncVectorRelational
{
    int32_t TestNot()
    {
        int32_t error = 0;

        {
            bool b(false);
            error += TRAP::Math::Not(b) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2b v(false);
            error += TRAP::Math::All(TRAP::Math::Not(v)) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3b v(false);
            error += TRAP::Math::All(TRAP::Math::Not(v)) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4b v(false);
            error += TRAP::Math::All(TRAP::Math::Not(v)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestLess()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Vec2 a(1, 2);
            const TRAP::Math::Vec2 b(2, 3);
            error += TRAP::Math::All(TRAP::Math::LessThan(a, b)) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::LessThanEqual(a, b)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3 a(1, 2, 3);
            const TRAP::Math::Vec3 b(2, 3, 4);
            error += TRAP::Math::All(TRAP::Math::LessThan(a, b)) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::LessThanEqual(a, b)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 a(1, 2, 3, 4);
            const TRAP::Math::Vec4 b(2, 3, 4, 5);
            error += TRAP::Math::All(TRAP::Math::LessThan(a, b)) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::LessThanEqual(a, b)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec2i a(1, 2);
            const TRAP::Math::Vec2i b(2, 3);
            error += TRAP::Math::All(TRAP::Math::LessThan(a, b)) ? 0 : 1;

            const TRAP::Math::Vec2i c(1, 3);
            error += TRAP::Math::All(TRAP::Math::LessThanEqual(a, c)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3i a(1, 2, 3);
            const TRAP::Math::Vec3i b(2, 3, 4);
            error += TRAP::Math::All(TRAP::Math::LessThan(a, b)) ? 0 : 1;

            const TRAP::Math::Vec3i c(1, 3, 4);
            error += TRAP::Math::All(TRAP::Math::LessThanEqual(a, c)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4i a(1, 2, 3, 4);
            const TRAP::Math::Vec4i b(2, 3, 4, 5);
            error += TRAP::Math::All(TRAP::Math::LessThan(a, b)) ? 0 : 1;

            const TRAP::Math::Vec4i c(1, 3, 4, 5);
            error += TRAP::Math::All(TRAP::Math::LessThanEqual(a, c)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestGreater()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Vec2 a(1, 2);
            const TRAP::Math::Vec2 b(2, 3);
            error += TRAP::Math::All(TRAP::Math::GreaterThan(b, a)) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::GreaterThanEqual(b, a)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3 a(1, 2, 3);
            const TRAP::Math::Vec3 b(2, 3, 4);
            error += TRAP::Math::All(TRAP::Math::GreaterThan(b, a)) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::GreaterThanEqual(b, a)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 a(1, 2, 3, 4);
            const TRAP::Math::Vec4 b(2, 3, 4, 5);
            error += TRAP::Math::All(TRAP::Math::GreaterThan(b, a)) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::GreaterThanEqual(b, a)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec2i a(1, 2);
            const TRAP::Math::Vec2i b(2, 3);
            error += TRAP::Math::All(TRAP::Math::GreaterThan(b, a)) ? 0 : 1;

            const TRAP::Math::Vec2i c(1, 3);
            error += TRAP::Math::All(TRAP::Math::GreaterThanEqual(c, a)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3i a(1, 2, 3);
            const TRAP::Math::Vec3i b(2, 3, 4);
            error += TRAP::Math::All(TRAP::Math::GreaterThan(b, a)) ? 0 : 1;

            const TRAP::Math::Vec3i c(1, 3, 4);
            error += TRAP::Math::All(TRAP::Math::GreaterThanEqual(c, a)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4i a(1, 2, 3, 4);
            const TRAP::Math::Vec4i b(2, 3, 4, 5);
            error += TRAP::Math::All(TRAP::Math::GreaterThan(b, a)) ? 0 : 1;

            const TRAP::Math::Vec4i c(1, 3, 4, 5);
            error += TRAP::Math::All(TRAP::Math::GreaterThanEqual(c, a)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestEqual()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Vec2i a(1, 2);
            const TRAP::Math::Vec2i b(1, 2);
            error += TRAP::Math::All(TRAP::Math::Equal(b, a)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3i a(1, 2, 3);
            const TRAP::Math::Vec3i b(1, 2, 3);
            error += TRAP::Math::All(TRAP::Math::Equal(b, a)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4i a(1, 2, 3, 4);
            const TRAP::Math::Vec4i b(1, 2, 3, 4);
            error += TRAP::Math::All(TRAP::Math::Equal(b, a)) ? 0 : 1;
        }

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunFuncVectorRelationalTests()
{
    int32_t error = 0;

    error += FuncVectorRelational::TestNot();
    error += FuncVectorRelational::TestLess();
    error += FuncVectorRelational::TestGreater();
    error += FuncVectorRelational::TestEqual();

    return error;
}