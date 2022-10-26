#include "TypeVec3.h"

#include <TRAP.h>

namespace TypeVec3
{
    int32_t TestVec3Constructor()
    {
        int32_t error = 0;

        {
            TRAP::Math::Vec3 a{0, 1, 2};
            std::vector<TRAP::Math::Vec3> v =
            {
                {0, 1, 2},
                {4, 5, 6},
                {8, 9, 0}
            };
        }

        {
            TRAP::Math::Vec3d a{0, 1, 2};
            std::vector<TRAP::Math::Vec3d> v =
            {
                {0, 1, 2},
                {4, 5, 6},
                {8, 9, 0}
            };
        }

        {
            TRAP::Math::Vec3i a(1);
            TRAP::Math::Vec3i b(1, 1, 1);

            error += a == b ? 0 : 1;
        }

        {
            std::vector<TRAP::Math::Vec3i> tests;
            tests.push_back(TRAP::Math::Vec3i(TRAP::Math::Vec2i(1, 2), 3));
            tests.push_back(TRAP::Math::Vec3i(1, TRAP::Math::Vec2i(2, 3)));
            tests.push_back(TRAP::Math::Vec3i(1, 2 ,3));
            tests.push_back(TRAP::Math::Vec3i(TRAP::Math::Vec4i(1, 2, 3, 4)));

            for(std::size_t i = 0; i < tests.size(); ++i)
                error += tests[i] == TRAP::Math::Vec3i(1, 2, 3) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3 o(1.0f, 2.0f, 3.0f);

            const TRAP::Math::Vec3 a(1.0f);
            const TRAP::Math::Vec3 b(1.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(a, b, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 c(1.0f, 2.0f, 3.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(c, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3 o(1.0f, 2.0f, 3.0f);

            const TRAP::Math::Vec3 a(1.0);
            const TRAP::Math::Vec3 b(1.0);
            error += TRAP::Math::All(TRAP::Math::Equal(a, b, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 c(1.0, 2.0, 3.0);
            error += TRAP::Math::All(TRAP::Math::Equal(c, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestVec3bConstructor()
    {
        int32_t error = 0;

        const TRAP::Math::Vec3b a(true);
        const TRAP::Math::Vec3b b(true);
        const TRAP::Math::Vec3b c(false);
        const TRAP::Math::Vec3b d = a && b;
        const TRAP::Math::Vec3b e = a && c;
        const TRAP::Math::Vec3b f = a || c;

        error += d == TRAP::Math::Vec3b(true) ? 0 : 1;
        error += e == TRAP::Math::Vec3b(false) ? 0 : 1;
        error += f == TRAP::Math::Vec3b(true) ? 0 : 1;

        const bool g = a == c;
        const bool h = a != c;
        error += !g ? 0 : 1;
        error += h ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestVec3Operators()
    {
        int32_t error = 0;

        {
            TRAP::Math::Vec3i a(1);
            TRAP::Math::Vec3i b(1);
            bool r = a != b;
            bool s = a == b;

            error += (s && !r) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec3 a(1.0f, 2.0f, 3.0f);
            const TRAP::Math::Vec3 b(4.0f, 5.0f, 6.0f);

            const TRAP::Math::Vec3 c = a + b;
            error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec3(5, 7, 9), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 d = b - a;
            error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec3(3, 3, 3), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 e = a * b;
            error += TRAP::Math::All(TRAP::Math::Equal(e, TRAP::Math::Vec3(4, 10, 18), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 f = b / a;
            error += TRAP::Math::All(TRAP::Math::Equal(f, TRAP::Math::Vec3(4, 2.5, 2), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 g = a + 1.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(g, TRAP::Math::Vec3(2, 3, 4), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 h = b - 1.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(h, TRAP::Math::Vec3(3, 4, 5), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 i = a * 2.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(i, TRAP::Math::Vec3(2, 4, 6), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 j = b / 2.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(j, TRAP::Math::Vec3(2, 2.5, 3), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 k = 1.0f + a;
            error += TRAP::Math::All(TRAP::Math::Equal(k, TRAP::Math::Vec3(2, 3, 4), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 l = 1.0f - b;
            error += TRAP::Math::All(TRAP::Math::Equal(l, TRAP::Math::Vec3(-3, -4, -5), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 m = 2.0f * a;
            error += TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Vec3(2, 4, 6), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec3 n = 2.0f / b;
            error += TRAP::Math::All(TRAP::Math::Equal(n, TRAP::Math::Vec3(0.5, 2.0 / 5.0, 2.0 / 6.0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3i a(1.0f, 2.0f, 3.0f);
            TRAP::Math::Vec3i b(4.0f, 5.0f, 6.0f);

            a += b;
            error += a == TRAP::Math::Vec3i(5, 7, 9) ? 0 : 1;

            a += 1;
            error += a == TRAP::Math::Vec3i(6, 8, 10) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3i a(1.0f, 2.0f, 3.0f);
            TRAP::Math::Vec3i b(4.0f, 5.0f, 6.0f);

            b -= a;
            error += b == TRAP::Math::Vec3i(3, 3, 3) ? 0 : 1;

            b -= 1;
            error += b == TRAP::Math::Vec3i(2, 2, 2) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3i a(1.0f, 2.0f, 3.0f);
            TRAP::Math::Vec3i b(4.0f, 5.0f, 6.0f);

            a *= b;
            error += a == TRAP::Math::Vec3i(4, 10, 18) ? 0 : 1;

            a *= 2;
            error += a == TRAP::Math::Vec3i(8, 20, 36) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3i a(1.0f, 2.0f, 3.0f);
            TRAP::Math::Vec3i b(4.0f, 5.0f, 6.0f);

            b /= a;
            error += b == TRAP::Math::Vec3i(4, 2, 2) ? 0 : 1;

            b /= 2;
            error += b == TRAP::Math::Vec3i(2, 1, 1) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3i b(2);

            b /= b.y;
            error += b == TRAP::Math::Vec3i(1) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3i a(1.0f, 2.0f, 3.0f);
            TRAP::Math::Vec3i b = -a;
            error += b == TRAP::Math::Vec3i(-1.0f, -2.0f, -3.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3i a(1.0f, 2.0f, 3.0f);
            TRAP::Math::Vec3i b = --a;
            error += b == TRAP::Math::Vec3i(0.0f, 1.0f, 2.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3i a(1.0f, 2.0f, 3.0f);
            TRAP::Math::Vec3i b = a--;
            error += b == TRAP::Math::Vec3i(1.0f, 2.0f, 3.0f) ? 0 : 1;
            error += a == TRAP::Math::Vec3i(0.0f, 1.0f, 2.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3i a(1.0f, 2.0f, 3.0f);
            TRAP::Math::Vec3i b = ++a;
            error += b == TRAP::Math::Vec3i(2.0f, 3.0f, 4.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec3i a(1.0f, 2.0f, 3.0f);
            TRAP::Math::Vec3i b = a++;
            error += b == TRAP::Math::Vec3i(1.0f, 2.0f, 3.0f) ? 0 : 1;
            error += a == TRAP::Math::Vec3i(2.0f, 3.0f, 4.0f) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestVec3Size()
    {
        int32_t error = 0;

        error += 12 == sizeof(TRAP::Math::Vec3) ? 0 : 1;
        error += 24 == sizeof(TRAP::Math::Vec3d) ? 0 : 1;
        error += TRAP::Math::Vec3().Length() == 3 ? 0 : 1;
        error += TRAP::Math::Vec3d().Length() == 3 ? 0 : 1;
        error += TRAP::Math::Vec3::Length() == 3 ? 0 : 1;
        error += TRAP::Math::Vec3d::Length() == 3 ? 0 : 1;

        static constexpr std::size_t length = TRAP::Math::Vec3::Length();
        error += length == 3 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestOperatorIncrement()
    {
        int32_t error = 0;

        TRAP::Math::Vec3i v0(1);
        TRAP::Math::Vec3i v1(v0);
        TRAP::Math::Vec3i v2(v0);
        TRAP::Math::Vec3i v3 = ++v1;
        TRAP::Math::Vec3i v4 = v2++;

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

    void TestConstexpr()
    {
        static_assert(TRAP::Math::Vec3::Length() == 3, "MathTests: Failed constexpr");
        static_assert(TRAP::Math::Vec3(1.0f).x > 0.0f, "MathTests: Failed constexpr");
        static_assert(TRAP::Math::Vec3(1.0f, -1.0f, -1.0f).x > 0.0f, "MathTests: Failed constexpr");
        static_assert(TRAP::Math::Vec3(1.0f, -1.0f, -1.0f).y < 0.0f, "MathTests: Failed constexpr");
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunTypeVec3Tests()
{
    int32_t error = 0;

    error += TypeVec3::TestVec3Constructor();
    error += TypeVec3::TestVec3bConstructor();
    error += TypeVec3::TestVec3Operators();
    error += TypeVec3::TestVec3Size();
    error += TypeVec3::TestOperatorIncrement();
    TypeVec3::TestConstexpr();

    return error;
}