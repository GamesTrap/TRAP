#include "Constexpr.h"

#include <TRAP.h>

namespace Constexpr
{
    void TestVec2()
    {
        {
            static constexpr TRAP::Math::Vec2b b(true);
            static constexpr bool a = TRAP::Math::All(b);
            static_assert(a, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec2b d(true, false);
            static constexpr bool c = TRAP::Math::Any(d);
            static_assert(c, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2b a(true);
            static constexpr TRAP::Math::Vec2b b(true, false);
            static_assert(TRAP::Math::Any(TRAP::Math::Equal(a, b)), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(1);
            static_assert(TRAP::Math::Vec2i(1) == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(1, 2);
            static_assert(TRAP::Math::Vec2i(1, 2) == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(TRAP::Math::Vec2(1, 2));
            static_assert(TRAP::Math::Vec2i(1, 2) == a, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec2i b(TRAP::Math::Vec3(1, 2, 3));
            static_assert(TRAP::Math::Vec2i(1, 2) == b, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec2i c(TRAP::Math::Vec4(1, 2, 3, 4));
            static_assert(TRAP::Math::Vec2i(1, 2) == c, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(1);
            static_assert(a[0] == 1, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec2(1.0f).x > 0.0f, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec2(1.0f, -1.0f).x > 0.0f, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec2(1.0f, -1.0f).y < 0.0f, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec2::Length() == 2, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2b a1(true);
            static constexpr TRAP::Math::Vec2b a2(true);
            static constexpr TRAP::Math::Vec2b b1(false);
            static constexpr TRAP::Math::Vec2b b2(false);
            static_assert(a1 == a2 && b1 == b2, "Math: Failed constexpr");
            static_assert(a1 == a2 || b1 == b2, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(1);
            static constexpr TRAP::Math::Vec2i b = a + 1;
            static constexpr TRAP::Math::Vec2i c(3);
            static_assert(a + b == c, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec2i d = +a;
            static_assert(d == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(3);
            static constexpr TRAP::Math::Vec2i b = a - 1;
            static constexpr TRAP::Math::Vec2i c(1);
            static_assert(a - b == c, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec2i d = -a;
            static_assert(-d == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(3);
            static constexpr TRAP::Math::Vec2i b = a * 1;
            static_assert(a == b, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec2i c(1);
            static_assert(b * c == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(3);
            static constexpr TRAP::Math::Vec2i b = a / 1;
            static_assert(a == b, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec2i c(1);
            static_assert(b / c == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(3);
            static constexpr TRAP::Math::Vec2i b = a % 2;
            static constexpr TRAP::Math::Vec2i c(1);
            static_assert(b == c, "Math: Failed constexpr");

            static_assert(a % 2 == c, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(1);
            static constexpr TRAP::Math::Vec2i b = a & 1;
            static_assert(a == b, "Math: Failed constexpr");

            static_assert(a == (a & 1), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(1);
            static constexpr TRAP::Math::Vec2i b = a | 1;
            static_assert(a == b, "Math: Failed constexpr");

            static_assert(a == (a | 1), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(1);
            static constexpr TRAP::Math::Vec2i b = a ^ 0;
            static_assert(a == b, "Math: Failed constexpr");

            static_assert(a == (a ^ 0), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(1);
            static constexpr TRAP::Math::Vec2i b = a << 1;
            static_assert(b == TRAP::Math::Vec2i(2), "Math: Failed constexpr");

            static_assert(b == (a << 1), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(2);
            static constexpr TRAP::Math::Vec2i b = a >> 1;
            static_assert(b == TRAP::Math::Vec2i(1), "Math: Failed constexpr");

            static_assert(b == a >> 1, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec2i a(~0);
            static constexpr TRAP::Math::Vec2i b = ~a;
            static_assert(a == ~b, "Math: Failed constexpr");
        }
    }

//-------------------------------------------------------------------------------------------------------------------//

    void TestVec3()
    {
        {
            static constexpr TRAP::Math::Vec3b b(true);
            static constexpr bool a = TRAP::Math::All(b);
            static_assert(a, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec3b d(true, false, true);
            static constexpr bool c = TRAP::Math::Any(d);
            static_assert(c, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3b a(true);
            static constexpr TRAP::Math::Vec3b b(true, false, true);
            static_assert(TRAP::Math::Any(TRAP::Math::Equal(a, b)), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(1);
            static_assert(TRAP::Math::Vec3i(1) == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(TRAP::Math::Vec2i(1, 2), 3);
            static_assert(TRAP::Math::Vec3i(1, 2, 3) == a, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec3i b(1, TRAP::Math::Vec2i(2, 3));
            static_assert(TRAP::Math::Vec3i(1, 2, 3) == b, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(1, 2, 3);
            static_assert(TRAP::Math::Vec3i(1, 2, 3) == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(TRAP::Math::Vec4i(1, 2, 3, 4));
            static_assert(TRAP::Math::Vec3i(1, 2, 3) == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(1);
            static_assert(a[0] == 1, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec3(1.0f).x > 0.0f, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec3(1.0f, -1.0f, -1.0f).x > 0.0f, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec3(1.0f, -1.0f, -1.0f).y < 0.0f, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec3::Length() == 3, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3b a1(true);
            static constexpr TRAP::Math::Vec3b a2(true);
            static constexpr TRAP::Math::Vec3b b1(false);
            static constexpr TRAP::Math::Vec3b b2(false);
            static_assert(a1 == a2 && b1 == b2, "Math: Failed constexpr");
            static_assert(a1 == a2 || b1 == b2, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(1);
            static constexpr TRAP::Math::Vec3i b = a + 1;
            static constexpr TRAP::Math::Vec3i c(3);
            static_assert(a + b == c, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec3i d = +a;
            static_assert(d == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(3);
            static constexpr TRAP::Math::Vec3i b = a - 1;
            static constexpr TRAP::Math::Vec3i c(1);
            static_assert(a - b == c, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec3i d = -a;
            static_assert(-d == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(3);
            static constexpr TRAP::Math::Vec3i b = a * 1;
            static_assert(a == b, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec3i c(1);
            static_assert(b * c == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(3);
            static constexpr TRAP::Math::Vec3i b = a / 1;
            static_assert(a == b, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec3i c(1);
            static_assert(b / c == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(3);
            static constexpr TRAP::Math::Vec3i b = a % 2;
            static constexpr TRAP::Math::Vec3i c(1);
            static_assert(b == c, "Math: Failed constexpr");

            static_assert(a % 2 == c, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(1);
            static constexpr TRAP::Math::Vec3i b = a & 1;
            static_assert(a == b, "Math: Failed constexpr");

            static_assert(a == (a & 1), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(1);
            static constexpr TRAP::Math::Vec3i b = a | 1;
            static_assert(a == b, "Math: Failed constexpr");

            static_assert(a == (a | 1), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(1);
            static constexpr TRAP::Math::Vec3i b = a ^ 0;
            static_assert(a == b, "Math: Failed constexpr");

            static_assert(a == (a ^ 0), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(1);
            static constexpr TRAP::Math::Vec3i b = a << 1;
            static_assert(b == TRAP::Math::Vec3i(2), "Math: Failed constexpr");

            static_assert(b == (a << 1), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(2);
            static constexpr TRAP::Math::Vec3i b = a >> 1;
            static_assert(b == TRAP::Math::Vec3i(1), "Math: Failed constexpr");

            static_assert(b == a >> 1, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec3i a(~0);
            static constexpr TRAP::Math::Vec3i b = ~a;
            static_assert(a == ~b, "Math: Failed constexpr");
        }
    }

//-------------------------------------------------------------------------------------------------------------------//

    void TestVec4()
    {
        {
            static constexpr TRAP::Math::Vec4b b(true);
            static constexpr bool a = TRAP::Math::All(b);
            static_assert(a, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4b d(true, false, true, false);
            static constexpr bool c = TRAP::Math::Any(d);
            static_assert(c, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4b a(true);
            static constexpr TRAP::Math::Vec4b b(true, false, true, false);
            static_assert(TRAP::Math::Any(TRAP::Math::Equal(a, b)), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i o(TRAP::Math::Vec4i(1));
            static_assert(TRAP::Math::Vec4i(1) == o, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i a(1);
            static_assert(TRAP::Math::Vec4i(1) == a, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i n(TRAP::Math::Vec4i(1, 2, 3, 4));
            static_assert(TRAP::Math::Vec4i(1, 2, 3, 4) == n, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(TRAP::Math::Vec3i(1, 2, 3), 4);
            static_assert(TRAP::Math::Vec4i(1, 2, 3, 4) == a, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i b(TRAP::Math::Vec2i(1, 2), TRAP::Math::Vec2i(3, 4));
            static_assert(TRAP::Math::Vec4i(1, 2, 3, 4) == b, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i c(1, TRAP::Math::Vec3i(2, 3, 4));
            static_assert(TRAP::Math::Vec4i(1, 2, 3, 4) == c, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i d(1, TRAP::Math::Vec2i(2, 3), 4);
            static_assert(TRAP::Math::Vec4i(1, 2, 3, 4) == d, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i e(TRAP::Math::Vec2i(1, 2), 3, 4);
            static_assert(TRAP::Math::Vec4i(1, 2, 3, 4) == e, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i f(1, 2, TRAP::Math::Vec2i(3, 4));
            static_assert(TRAP::Math::Vec4i(1, 2, 3, 4) == f, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(1);
            static_assert(a[0] == 1, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec4i(1).x > 0, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec4i(1.0f, -1.0f, -1.0f, 1.0f).x > 0, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec4i(1.0f, -1.0f, -1.0f, 1.0f).y < 0, "Math: Failed constexpr");
            static_assert(TRAP::Math::Vec4::Length() == 4, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4b a1(true);
            static constexpr TRAP::Math::Vec4b a2(true);
            static constexpr TRAP::Math::Vec4b b1(false);
            static constexpr TRAP::Math::Vec4b b2(false);
            static_assert(a1 == a2 && b1 == b2, "Math: Failed constexpr");
            static_assert(a1 == a2 || b1 == b2, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(1);
            static constexpr TRAP::Math::Vec4i b = a + 1;
            static constexpr TRAP::Math::Vec4i c(3);
            static_assert(a + b == c, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i d = +a;
            static_assert(d == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(3);
            static constexpr TRAP::Math::Vec4i b = a - 1;
            static constexpr TRAP::Math::Vec4i c(1);
            static_assert(a - b == c, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i d = -a;
            static_assert(-d == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(3);
            static constexpr TRAP::Math::Vec4i b = a * 1;
            static_assert(a == b, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i c(1);
            static_assert(b * c == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(3);
            static constexpr TRAP::Math::Vec4i b = a / 1;
            static_assert(a == b, "Math: Failed constexpr");

            static constexpr TRAP::Math::Vec4i c(1);
            static_assert(b / c == a, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(3);
            static constexpr TRAP::Math::Vec4i b = a % 2;
            static constexpr TRAP::Math::Vec4i c(1);
            static_assert(b == c, "Math: Failed constexpr");

            static_assert(a % 2 == c, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(1);
            static constexpr TRAP::Math::Vec4i b = a & 1;
            static_assert(a == b, "Math: Failed constexpr");

            static_assert(a == (a & 1), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(1);
            static constexpr TRAP::Math::Vec4i b = a | 1;
            static_assert(a == b, "Math: Failed constexpr");

            static_assert(a == (a | 1), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(1);
            static constexpr TRAP::Math::Vec4i b = a ^ 0;
            static_assert(a == b, "Math: Failed constexpr");

            static_assert(a == (a ^ 0), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(1);
            static constexpr TRAP::Math::Vec4i b = a << 1;
            static_assert(b == TRAP::Math::Vec4i(2), "Math: Failed constexpr");

            static_assert(b == (a << 1), "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(2);
            static constexpr TRAP::Math::Vec4i b = a >> 1;
            static_assert(b == TRAP::Math::Vec4i(1), "Math: Failed constexpr");

            static_assert(b == a >> 1, "Math: Failed constexpr");
        }

        {
            static constexpr TRAP::Math::Vec4i a(~0);
            static constexpr TRAP::Math::Vec4i b = ~a;
            static_assert(a == ~b, "Math: Failed constexpr");
        }
    }

//-------------------------------------------------------------------------------------------------------------------//

    void TestQuaternion()
    {
        {
            static_assert(TRAP::Math::Quat::Length() == 4, "Math: Failed constexpr");
            static_assert(TRAP::Math::Quat(1.0f, TRAP::Math::Vec3(0.0f)).w > 0.0f, "Math: Failed constexpr");
            static_assert(TRAP::Math::Quat(1.0f, 0.0f, 0.0f, 0.0f).w > 0.0f, "Math: Failed constexpr");

            static constexpr TRAP::Math::Quat q = TRAP::Math::Quat();
            static_assert(q.x - TRAP::Math::Quat(1.0f, TRAP::Math::Vec3(0.0f)).x <= TRAP::Math::Epsilon<float>(), "Math: Failed constexpr");
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void RunConstexprTests()
{
    Constexpr::TestVec2();
    Constexpr::TestVec3();
    Constexpr::TestVec4();
    Constexpr::TestQuaternion();
}