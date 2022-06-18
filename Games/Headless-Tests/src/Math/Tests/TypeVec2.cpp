#include "TypeVec2.h"

#include <TRAP.h>

namespace TypeVec2
{
    int32_t TestSize()
    {
        int32_t error = 0;

        error += 8 == sizeof(TRAP::Math::Vec2) ? 0 : 1;
        error += 16 == sizeof(TRAP::Math::Vec2d) ? 0 : 1;
        error += TRAP::Math::Vec2().Length() == 2 ? 0 : 1;
        error += TRAP::Math::Vec2d().Length() == 2 ? 0 : 1;
        error += TRAP::Math::Vec2::Length() == 2 ? 0 : 1;
        error += TRAP::Math::Vec2d::Length() == 2 ? 0 : 1;

        constexpr std::size_t length = TRAP::Math::Vec2::Length();
        error += length == 2 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestConstructor()
    {
        int32_t error = 0;

        {
            TRAP::Math::Vec2i a(1);
            TRAP::Math::Vec2i b(a);
            error += a == b ? 0 : 1;
        }

        {
            TRAP::Math::Vec2 a{0, 1};
            std::vector<TRAP::Math::Vec2> v =
            {
                {0, 1},
                {4, 5},
                {8, 9}
            };
        }

        {
            TRAP::Math::Vec2d a{0, 1};
            std::vector<TRAP::Math::Vec2d> v =
            {
                {0, 1},
                {4, 5},
                {8, 9}
            };
        }

        {
            TRAP::Math::Vec2 a = TRAP::Math::Vec2(2.0f);
            TRAP::Math::Vec2 b = TRAP::Math::Vec2(2.0f, 3.0f);
            TRAP::Math::Vec2 c = TRAP::Math::Vec2(2.0f, 3.0);
            TRAP::Math::Vec2d d = TRAP::Math::Vec2d(2.0);
            TRAP::Math::Vec2d e(TRAP::Math::Vec2d(2.0f));
            TRAP::Math::Vec2d f(TRAP::Math::Vec2i(2));
        }

        {
            const TRAP::Math::Vec2 o(1.0f, 2.0f);

            const TRAP::Math::Vec2 a(1.0f);
            const TRAP::Math::Vec2 b(1.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(a, b, 0.0001f)) ? 0 : 1;

            const TRAP::Math::Vec2 c(1.0f, 2.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(c, o, 0.0001f)) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec2 o(1.0, 2.0f);

            const TRAP::Math::Vec2 a(1.0f);
            const TRAP::Math::Vec2 b(1.0);
            error += TRAP::Math::All(TRAP::Math::Equal(a, b, 0.0001f)) ? 0 : 1;

            const TRAP::Math::Vec2 c(1.0f, 2.0);
            error += TRAP::Math::All(TRAP::Math::Equal(c, o, 0.0001f)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestOperators()
    {
        int32_t error = 0;

        {
            TRAP::Math::Vec2i a(1);
            TRAP::Math::Vec2i b(1);
            error += a != b ? 1 : 0;
            error += a == b ? 0 : 1;
        }

        {
            TRAP::Math::Vec2 a(1.0f);
            TRAP::Math::Vec2 c = a + 1.0f;
            a += 1.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec2(2.0f), TRAP::Math::Epsilon<float>())) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(a, c, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2 a(1.0f);
            TRAP::Math::Vec2 b(2.0f, -1.0f);
            TRAP::Math::Vec2 c = a + b;
            a += b;
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec2(3.0f, 0.0f), TRAP::Math::Epsilon<float>())) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(a, c, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2 a(1.0f);
            TRAP::Math::Vec2 c = a - 1.0f;
            a -= 1.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec2(0.0f), TRAP::Math::Epsilon<float>())) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(a, c, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2 a(1.0f);
            TRAP::Math::Vec2 b(2.0f, -1.0f);
            TRAP::Math::Vec2 c = a - b;
            a -= b;
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec2(-1.0f, 2.0f), TRAP::Math::Epsilon<float>())) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(a, c, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2 a(1.0f);
            TRAP::Math::Vec2 c = a * 2.0f;
            a *= 2.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec2(2.0f), TRAP::Math::Epsilon<float>())) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(a, c, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2 a(2.0f);
            TRAP::Math::Vec2 b(2.0f);
            TRAP::Math::Vec2 c = a / b;
            a /= b;
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec2(1.0f), TRAP::Math::Epsilon<float>())) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(a, c, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2 a(1.0f, 2.0f);
            TRAP::Math::Vec2 b(4.0f, 5.0f);

            TRAP::Math::Vec2 c = a + b;
            error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec2(5, 7), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 d = b - a;
            error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec2(3, 3), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 e = a * b;
            error += TRAP::Math::All(TRAP::Math::Equal(e, TRAP::Math::Vec2(4, 10), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 f = b / a;
            error += TRAP::Math::All(TRAP::Math::Equal(f, TRAP::Math::Vec2(4, 2.5), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 g = a + 1.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(g, TRAP::Math::Vec2(2, 3), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 h = b - 1.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(h, TRAP::Math::Vec2(3, 4), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 i = a * 2.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(i, TRAP::Math::Vec2(2, 4), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 j = b / 2.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(j, TRAP::Math::Vec2(2, 2.5), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 k = 1.0f + a;
            error += TRAP::Math::All(TRAP::Math::Equal(k, TRAP::Math::Vec2(2, 3), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 l = 1.0f - b;
            error += TRAP::Math::All(TRAP::Math::Equal(l, TRAP::Math::Vec2(-3, -4), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 m = 2.0f * a;
            error += TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Vec2(2, 4), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            TRAP::Math::Vec2 n = 2.0f / b;
            error += TRAP::Math::All(TRAP::Math::Equal(n, TRAP::Math::Vec2(0.5, 2.0 / 5.0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2 a(1.0f, 2.0f);
            TRAP::Math::Vec2 b(4.0f, 5.0f);

            a += b;
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec2(5, 7), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            a += 1.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(a, TRAP::Math::Vec2(6, 8), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2i a(1.0f, 2.0f);
            TRAP::Math::Vec2i b(4.0f, 5.0f);

            b -= a;
            error += b == TRAP::Math::Vec2i(3, 3) ? 0 : 1;

            b -= 1.0f;
            error += b == TRAP::Math::Vec2i(2, 2) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2i a(1.0f, 2.0f);
            TRAP::Math::Vec2i b(4.0f, 5.0f);

            a *= b;
            error += a == TRAP::Math::Vec2i(4, 10) ? 0 : 1;

            a *= 2;
            error += a == TRAP::Math::Vec2i(8, 20) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2i a(1.0f, 2.0f);
            TRAP::Math::Vec2i b(4.0f, 16.0f);

            b /= a;
            error += b == TRAP::Math::Vec2i(4, 8) ? 0 : 1;

            b /= 2.0f;
            error += b == TRAP::Math::Vec2i(2, 4) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2i b(2);

            b /= b.y;
            error += b == TRAP::Math::Vec2i(1) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2i a(1.0f, 2.0f);
            TRAP::Math::Vec2i b = -a;
            error += b == TRAP::Math::Vec2i(-1.0f, -2.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2i a(1.0f, 2.0f);
            TRAP::Math::Vec2i b = --a;
            error += b == TRAP::Math::Vec2i(0.0f, 1.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2i a(1.0f, 2.0f);
            TRAP::Math::Vec2i b = a--;
            error += b == TRAP::Math::Vec2i(1.0f, 2.0f) ? 0 : 1;
            error += a == TRAP::Math::Vec2i(0.0f, 1.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2i a(1.0f, 2.0f);
            TRAP::Math::Vec2i b = ++a;
            error += b == TRAP::Math::Vec2i(2.0f, 3.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec2i a(1.0f, 2.0f);
            TRAP::Math::Vec2i b = a++;
            error += b == TRAP::Math::Vec2i(1.0f, 2.0f) ? 0 : 1;
            error += a == TRAP::Math::Vec2i(2.0f, 3.0f) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestOperatorIncrement()
    {
        int32_t error = 0;

        TRAP::Math::Vec2i v0(1);
        TRAP::Math::Vec2i v1(v0);
        TRAP::Math::Vec2i v2(v0);
        TRAP::Math::Vec2i v3 = ++v1;
        TRAP::Math::Vec2i v4 = v2++;

        error += TRAP::Math::All(TRAP::Math::Equal(v0, v4)) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(v1, v2)) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(v1, v3)) ? 0 : 1;

        int32_t i0(1);
        int32_t i1(i0);
        int32_t i2(i0);
        int32_t i3 = ++i1;
        int32_t i4 = i2++;

        error += i0 == i4 ? 0 : 1;
        error += i1 == i2 ? 0 : 1;
        error += i1 == i3 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestSwizzle()
    {
        int32_t error = 0;

        //TODO

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    void TestConstexpr()
    {
        static_assert(TRAP::Math::Vec2::Length() == 2, "MathTests: Failed constexpr");
        static_assert(TRAP::Math::Vec2(1.0f).x > 0.0f, "MathTests: Failed constexpr");
        static_assert(TRAP::Math::Vec2(1.0f, -1.0f).x > 0.0f, "MathTests: Failed constexpr");
        static_assert(TRAP::Math::Vec2(1.0f, -1.0f).y < 0.0f, "MathTests: Failed constexpr");
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunTypeVec2Tests()
{
    int32_t error = 0;

    error += TypeVec2::TestSize();
    error += TypeVec2::TestConstructor();
    error += TypeVec2::TestOperators();
    error += TypeVec2::TestOperatorIncrement();
    error += TypeVec2::TestSwizzle();
    TypeVec2::TestConstexpr();

    return error;
}