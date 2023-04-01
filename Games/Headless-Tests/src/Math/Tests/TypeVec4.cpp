#include "TypeVec4.h"

#include <TRAP.h>

namespace TypeVec4
{
    int32_t TestVec4Constructor()
    {
        int32_t error = 0;

        {
            TRAP::Math::Vec4i a(1, 2, 3, 4);
            TRAP::Math::Vec4i b(a);
            error += TRAP::Math::All(TRAP::Math::Equal(a, b)) ? 0 : 1;
        }

        {
            [[maybe_unused]] TRAP::Math::Vec4 a{0, 1, 2, 3};
            std::vector<TRAP::Math::Vec4> v =
            {
                {0, 1, 2, 3},
                {4, 5, 6, 7},
                {8, 9, 0, 1}
            };
        }

        {
            [[maybe_unused]] TRAP::Math::Vec4d a{0, 1, 2, 3};
            std::vector<TRAP::Math::Vec4d> v =
            {
                {0, 1, 2, 3},
                {4, 5, 6, 7},
                {8, 9, 0, 1}
            };
        }

        {
            const TRAP::Math::Vec4i a(1);
            const TRAP::Math::Vec4i b(1, 1, 1, 1);

            error += a == b ? 0 : 1;
        }

        {
            std::vector<TRAP::Math::Vec4i> tests;
            tests.push_back(TRAP::Math::Vec4i(TRAP::Math::Vec2i(1, 2), 3, 4));
            tests.push_back(TRAP::Math::Vec4i(1, TRAP::Math::Vec2i(2, 3), 4));
            tests.push_back(TRAP::Math::Vec4i(1, 2, TRAP::Math::Vec2i(3, 4)));
            tests.push_back(TRAP::Math::Vec4i(TRAP::Math::Vec3i(1, 2, 3), 4));
            tests.push_back(TRAP::Math::Vec4i(1, TRAP::Math::Vec3i(2, 3, 4)));
            tests.push_back(TRAP::Math::Vec4i(TRAP::Math::Vec2i(1, 2), TRAP::Math::Vec2i(3, 4)));
            tests.push_back(TRAP::Math::Vec4i(1, 2, 3, 4));
            tests.push_back(TRAP::Math::Vec4i(TRAP::Math::Vec4i(1, 2, 3, 4)));

            for(std::size_t i = 0; i < tests.size(); ++i)
                error += tests[i] == TRAP::Math::Vec4i(1, 2, 3, 4) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 o(1.0f, 2.0f, 3.0f, 4.0f);

            const TRAP::Math::Vec4 a(1.0f);
            const TRAP::Math::Vec4 b(1.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(a, b, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 c(1.0f, 2.0f, 3.0f, 4.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(c, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 o(1.0f, 2.0f, 3.0f, 4.0f);

            const TRAP::Math::Vec4 a(1.0f);
            const TRAP::Math::Vec4 b(1.0);
            error += TRAP::Math::All(TRAP::Math::Equal(a, b, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 c(1.0f, 2.0, 3.0f, 4.0);
            error += TRAP::Math::All(TRAP::Math::Equal(c, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec2 v2_0(1.0f, 2.0f);
            [[maybe_unused]] const TRAP::Math::Vec2 v2_1(2.0f, 3.0f);
            const TRAP::Math::Vec2 v2_2(3.0f, 4.0f);

            [[maybe_unused]] const TRAP::Math::Vec3 v3_0(1.0f, 2.0f, 3.0f);
            [[maybe_unused]] const TRAP::Math::Vec3 v3_1(2.0f, 3.0f, 4.0f);

            const TRAP::Math::Vec4 o(1.0f, 2.0, 3.0f, 4.0);

            const TRAP::Math::Vec4 a(1.0f, 2.0f, v2_2);
            error += TRAP::Math::All(TRAP::Math::Equal(a, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 b(1.0f, 2.0f, v2_2);
            error += TRAP::Math::All(TRAP::Math::Equal(b, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 c(v2_0, 3.0f, 4.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(c, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 d(v2_0, 3.0, 4.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(d, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 e(v2_0, 3.0f, 4.0);
            error += TRAP::Math::All(TRAP::Math::Equal(e, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec2 v2(2.0f, 3.0f);

            const TRAP::Math::Vec4 o(1.0f, 2.0, 3.0f, 4.0);

            const TRAP::Math::Vec4 a(1.0f, v2, 4.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(a, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 b(1.0f, v2, 4.0);
            error += TRAP::Math::All(TRAP::Math::Equal(b, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 c(1.0, v2, 4.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(c, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 d(1.0f, v2, 4.0);
            error += TRAP::Math::All(TRAP::Math::Equal(d, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 e(1.0, v2, 4.0f);
            error += TRAP::Math::All(TRAP::Math::Equal(e, o, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestVec4bConstructor()
    {
        int32_t error = 0;

        const TRAP::Math::Vec4b a(true);
        const TRAP::Math::Vec4b b(true);
        const TRAP::Math::Vec4b c(false);
        const TRAP::Math::Vec4b d = a && b;
        const TRAP::Math::Vec4b e = a && c;
        const TRAP::Math::Vec4b f = a || c;

        error += d == TRAP::Math::Vec4b(true) ? 0 : 1;
        error += e == TRAP::Math::Vec4b(false) ? 0 : 1;
        error += f == TRAP::Math::Vec4b(true) ? 0 : 1;

        const bool g = a == c;
        const bool h = a != c;
        error += !g ? 0 : 1;
        error += h ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestSize()
    {
        int32_t error = 0;

        error += 16 == sizeof(TRAP::Math::Vec4) ? 0 : 1;
        error += 32 == sizeof(TRAP::Math::Vec4d) ? 0 : 1;
        error += TRAP::Math::Vec4().Length() == 4 ? 0 : 1;
        error += TRAP::Math::Vec4d().Length() == 4 ? 0 : 1;
        error += TRAP::Math::Vec4::Length() == 4 ? 0 : 1;
        error += TRAP::Math::Vec4d::Length() == 4 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestOperators()
    {
        int32_t error = 0;

        {
            TRAP::Math::Vec4i a(1);
            TRAP::Math::Vec4i b(1);
            bool r = a != b;
            bool s = a == b;

            error += (s && !r) ? 0 : 1;
        }

        {
            const TRAP::Math::Vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
            const TRAP::Math::Vec4 b(4.0f, 5.0f, 6.0f, 7.0f);

            const TRAP::Math::Vec4 c = a + b;
            error += TRAP::Math::All(TRAP::Math::Equal(c, TRAP::Math::Vec4(5, 7, 9, 11), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 d = b - a;
            error += TRAP::Math::All(TRAP::Math::Equal(d, TRAP::Math::Vec4(3, 3, 3, 3), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 e = a * b;
            error += TRAP::Math::All(TRAP::Math::Equal(e, TRAP::Math::Vec4(4, 10, 18, 28), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 f = b / a;
            error += TRAP::Math::All(TRAP::Math::Equal(f, TRAP::Math::Vec4(4, 2.5, 2, 7.0f / 4.0f), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 g = a + 1.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(g, TRAP::Math::Vec4(2, 3, 4, 5), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 h = b - 1.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(h, TRAP::Math::Vec4(3, 4, 5, 6), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 i = a * 2.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(i, TRAP::Math::Vec4(2, 4, 6, 8), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 j = b / 2.0f;
            error += TRAP::Math::All(TRAP::Math::Equal(j, TRAP::Math::Vec4(2, 2.5, 3, 3.5), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 k = 1.0f + a;
            error += TRAP::Math::All(TRAP::Math::Equal(k, TRAP::Math::Vec4(2, 3, 4, 5), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 l = 1.0f - b;
            error += TRAP::Math::All(TRAP::Math::Equal(l, TRAP::Math::Vec4(-3, -4, -5, -6), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 m = 2.0f * a;
            error += TRAP::Math::All(TRAP::Math::Equal(m, TRAP::Math::Vec4(2, 4, 6, 8), TRAP::Math::Epsilon<float>())) ? 0 : 1;

            const TRAP::Math::Vec4 n = 2.0f / b;
            error += TRAP::Math::All(TRAP::Math::Equal(n, TRAP::Math::Vec4(0.5, 2.0 / 5.0, 2.0 / 6.0, 2.0 / 7.0), TRAP::Math::Epsilon<float>())) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4i a(1.0f, 2.0f, 3.0f, 4.0f);
            TRAP::Math::Vec4i b(4.0f, 5.0f, 6.0f, 7.0f);

            a += b;
            error += a == TRAP::Math::Vec4i(5, 7, 9, 11) ? 0 : 1;

            a += 1;
            error += a == TRAP::Math::Vec4i(6, 8, 10, 12) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4i a(1.0f, 2.0f, 3.0f, 4.0f);
            TRAP::Math::Vec4i b(4.0f, 5.0f, 6.0f, 7.0f);

            b -= a;
            error += b == TRAP::Math::Vec4i(3, 3, 3, 3) ? 0 : 1;

            b -= 1;
            error += b == TRAP::Math::Vec4i(2, 2, 2, 2) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4i a(1.0f, 2.0f, 3.0f, 4.0f);
            TRAP::Math::Vec4i b(4.0f, 5.0f, 6.0f, 7.0f);

            a *= b;
            error += a == TRAP::Math::Vec4i(4, 10, 18, 28) ? 0 : 1;

            a *= 2;
            error += a == TRAP::Math::Vec4i(8, 20, 36, 56) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4i a(1.0f, 2.0f, 2.0f, 4.0f);
            TRAP::Math::Vec4i b(4.0f, 4.0f, 8.0f, 8.0f);

            b /= a;
            error += b == TRAP::Math::Vec4i(4, 2, 4, 2) ? 0 : 1;

            b /= 2;
            error += b == TRAP::Math::Vec4i(2, 1, 2, 1) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4i b(2);

            b /= b.y;
            error += b == TRAP::Math::Vec4i(1) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4i a(1.0f, 2.0f, 3.0f, 4.0f);
            TRAP::Math::Vec4i b = -a;
            error += b == TRAP::Math::Vec4i(-1.0f, -2.0f, -3.0f, -4.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4i a(1.0f, 2.0f, 3.0f, 4.0f);
            TRAP::Math::Vec4i b = --a;
            error += b == TRAP::Math::Vec4i(0.0f, 1.0f, 2.0f, 3.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4i a(1.0f, 2.0f, 3.0f, 4.0f);
            TRAP::Math::Vec4i b = a--;
            error += b == TRAP::Math::Vec4i(1.0f, 2.0f, 3.0f, 4.0f) ? 0 : 1;
            error += a == TRAP::Math::Vec4i(0.0f, 1.0f, 2.0f, 3.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4i a(1.0f, 2.0f, 3.0f, 4.0f);
            TRAP::Math::Vec4i b = ++a;
            error += b == TRAP::Math::Vec4i(2.0f, 3.0f, 4.0f, 5.0f) ? 0 : 1;
        }

        {
            TRAP::Math::Vec4i a(1.0f, 2.0f, 3.0f, 4.0f);
            TRAP::Math::Vec4i b = a++;
            error += b == TRAP::Math::Vec4i(1.0f, 2.0f, 3.0f, 4.0f) ? 0 : 1;
            error += a == TRAP::Math::Vec4i(2.0f, 3.0f, 4.0f, 5.0f) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestEqual()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Vec4ui a(1, 2, 3, 4);
            const TRAP::Math::Vec4ui b(1, 2, 3, 4);

            error += a == b ? 0 : 1;
            error += a != b ? 1 : 0;
        }

        {
            const TRAP::Math::Vec4i a(1, 2, 3, 4);
            const TRAP::Math::Vec4i b(1, 2, 3, 4);

            error += a == b ? 0 : 1;
            error += a != b ? 1 : 0;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestOperatorIncrement()
    {
        int32_t error = 0;

        TRAP::Math::Vec4i v0(1);
        TRAP::Math::Vec4i v1(v0);
        TRAP::Math::Vec4i v2(v0);
        TRAP::Math::Vec4i v3 = ++v1;
        TRAP::Math::Vec4i v4 = v2++;

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
        static_assert(TRAP::Math::Vec4::Length() == 4, "MathTests: Failed constexpr");
        static_assert(TRAP::Math::Vec4(1.0f).x > 0.0f, "MathTests: Failed constexpr");
        static_assert(TRAP::Math::Vec4(1.0f, -1.0f, -1.0f, -1.0f).x > 0.0f, "MathTests: Failed constexpr");
        static_assert(TRAP::Math::Vec4(1.0f, -1.0f, -1.0f, -1.0f).y < 0.0f, "MathTests: Failed constexpr");
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunTypeVec4Tests()
{
    int32_t error = 0;

    error += TypeVec4::TestVec4Constructor();
    error += TypeVec4::TestVec4bConstructor();
    error += TypeVec4::TestSize();
    error += TypeVec4::TestOperators();
    error += TypeVec4::TestEqual();
    error += TypeVec4::TestOperatorIncrement();
    TypeVec4::TestConstexpr();

    return error;
}