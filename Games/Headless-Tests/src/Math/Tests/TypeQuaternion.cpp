#include "TypeQuaternion.h"

#include <TRAP.h>

namespace TypeQuaternion
{
    int32_t TestConstructor()
    {
        int32_t error = 0;

        {
            [[maybe_unused]] TRAP::Math::Quat a{0, 1, 2, 3};

            std::vector<TRAP::Math::Quat> b
            {
                {0, 1, 2, 3},
                {0, 1, 2, 3}
            };
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestTwoAxisConstructor()
    {
        int32_t error = 0;

        const TRAP::Math::Quat q1(TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Vec3(0, 1, 0));
        const TRAP::Math::Vec3 v1 = q1 * TRAP::Math::Vec3(1, 0, 0);
        error += TRAP::Math::All(TRAP::Math::Equal(v1, TRAP::Math::Vec3(0, 1, 0), 0.0001f)) ? 0 : 1;

        const TRAP::Math::Quat q2 = q1 * q1;
        const TRAP::Math::Vec3 v2 = q2 * TRAP::Math::Vec3(1, 0, 0);
        error += TRAP::Math::All(TRAP::Math::Equal(v2, TRAP::Math::Vec3(-1, 0, 0), 0.0001f)) ? 0 : 1;

        const TRAP::Math::Quat q3(TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Vec3(-1, 0, 0));
        const TRAP::Math::Vec3 v3 = q3 * TRAP::Math::Vec3(1, 0, 0);
        error += TRAP::Math::All(TRAP::Math::Equal(v3, TRAP::Math::Vec3(-1, 0, 0), 0.0001f)) ? 0 : 1;

        const TRAP::Math::Quat q4(TRAP::Math::Vec3(0, 1, 0), TRAP::Math::Vec3(0, -1, 0));
        const TRAP::Math::Vec3 v4 = q4 * TRAP::Math::Vec3(0, 1, 0);
        error += TRAP::Math::All(TRAP::Math::Equal(v4, TRAP::Math::Vec3(0, -1, 0), 0.0001f)) ? 0 : 1;

        const TRAP::Math::Quat q5(TRAP::Math::Vec3(0, 0, 1), TRAP::Math::Vec3(0, 0, -1));
        const TRAP::Math::Vec3 v5 = q5 * TRAP::Math::Vec3(0, 0, 1);
        error += TRAP::Math::All(TRAP::Math::Equal(v5, TRAP::Math::Vec3(0, 0, -1), 0.0001f)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestSize()
    {
        int32_t error = 0;

        const std::size_t a = sizeof(TRAP::Math::Quat);
        error += 16 == a ? 0 : 1;
        const std::size_t b = sizeof(TRAP::Math::Quatd);
        error += 32 == b ? 0 : 1;
        error += TRAP::Math::Quat().Length() == 4 ? 0 : 1;
        error += TRAP::Math::Quatd().Length() == 4 ? 0 : 1;
        error += TRAP::Math::Quat::Length() == 4 ? 0 : 1;
        error += TRAP::Math::Quatd::Length() == 4 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    void TestConstexpr()
    {
        static_assert(TRAP::Math::Quat::Length() == 4, "MathTests: Failed constexpr");
        static_assert(TRAP::Math::Quat(1.0f, TRAP::Math::Vec3(0.0f)).w > 0.0f, "MathTests: Failed constexpr");
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestConjugate()
    {
        int32_t error = 0;

        const TRAP::Math::Quat a(TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Vec3(0, 1, 0));
        const TRAP::Math::Quat c = TRAP::Math::Conjugate(a);
        error += TRAP::Math::Any(TRAP::Math::NotEqual(a, c, TRAP::Math::Epsilon<float>())) ? 0 : 1;

        const TRAP::Math::Quat b = TRAP::Math::Conjugate(c);
        error += TRAP::Math::All(TRAP::Math::Equal(a, b, TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMix()
    {
        int32_t error = 0;

        const TRAP::Math::Quat q1(TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Vec3(1, 0, 0));
        const TRAP::Math::Quat q2(TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Vec3(0, 1, 0));

        {
            const TRAP::Math::Quat q3 = TRAP::Math::Mix(q1, q2, 0.5f);
            const float f3 = TRAP::Math::Degrees(TRAP::Math::Angle(q3));
            error += TRAP::Math::Equal(f3, 45.0f, 0.001f) ? 0 : 1;

            const TRAP::Math::Quat q4 = TRAP::Math::Mix(q2, q1, 0.5f);
            const float f4 = TRAP::Math::Degrees(TRAP::Math::Angle(q4));
            error += TRAP::Math::Equal(f4, 45.0f, 0.001f) ? 0 : 1;
        }

        {
            const TRAP::Math::Quat q3 = TRAP::Math::SLerp(q1, q2, 0.5f);
            const float f3 = TRAP::Math::Degrees(TRAP::Math::Angle(q3));
            error += TRAP::Math::Equal(f3, 45.0f, 0.001f) ? 0 : 1;

            const TRAP::Math::Quat q4 = TRAP::Math::SLerp(q2, q1, 0.5f);
            const float f4 = TRAP::Math::Degrees(TRAP::Math::Angle(q4));
            error += TRAP::Math::Equal(f4, 45.0f, 0.001f) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    template<typename quatType, typename vecType>
    int32_t TestLog()
    {
        using T = typename quatType::valueType;

        const T epsilon = static_cast<T>(0.001f);

        int32_t error = 0;

        const quatType q(vecType(1, 0, 0), vecType(0, 1, 0));
        const quatType p = TRAP::Math::Log(q);
        error += TRAP::Math::Any(TRAP::Math::NotEqual(q, p, epsilon)) ? 0 : 1;

        const quatType r = TRAP::Math::Exp(p);
        error += TRAP::Math::All(TRAP::Math::Equal(q, r, epsilon)) ? 0 : 1;

        return error;
    }
//-------------------------------------------------------------------------------------------------------------------//

    template<typename quatType, typename vecType>
    int32_t TestPow()
    {
        using T = typename quatType::valueType;

        const T epsilon = static_cast<T>(0.001f);

        int32_t error = 0;

        const quatType q(vecType(1, 0, 0), vecType(0, 1, 0));

        {
            const T one = static_cast<T>(1.0f);
            const quatType p = TRAP::Math::Pow(q, one);
            error += TRAP::Math::All(TRAP::Math::Equal(q, p, epsilon)) ? 0 : 1;
        }

        {
            const T two = static_cast<T>(2.0f);
            const quatType p = TRAP::Math::Pow(q, two);
            const quatType r = q * q;
            error += TRAP::Math::All(TRAP::Math::Equal(p, r, epsilon)) ? 0 : 1;

            const quatType u = TRAP::Math::Sqrt(p);
            error += TRAP::Math::All(TRAP::Math::Equal(q, u, epsilon)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestLength()
    {
        int32_t error = 0;

        {
            const float a = TRAP::Math::Length(TRAP::Math::Quat(1, 0, 0, 0));
            error += TRAP::Math::Equal(a, 1.0f, 0.001f) ? 0 : 1;
        }

        {
            const float a = TRAP::Math::Length(TRAP::Math::Quat(1, TRAP::Math::Vec3(0)));
            error += TRAP::Math::Equal(a, 1.0f, 0.001f) ? 0 : 1;
        }

        {
            const float a = TRAP::Math::Length(TRAP::Math::Quat(TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Vec3(0, 1, 0)));
            error += TRAP::Math::Equal(a, 1.0f, 0.001f) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestNormalize()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Quat a = TRAP::Math::Quat(1, 0, 0, 0);
            const TRAP::Math::Quat n = TRAP::Math::Normalize(a);
            error += TRAP::Math::All(TRAP::Math::Equal(a, n, 0.001f)) ? 0 : 1;
        }

        {
            const TRAP::Math::Quat a = TRAP::Math::Quat(1, TRAP::Math::Vec3(0));
            const TRAP::Math::Quat n = TRAP::Math::Normalize(a);
            error += TRAP::Math::All(TRAP::Math::Equal(a, n, 0.001f)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestDot()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Quat a = TRAP::Math::Quat(1, 0, 0, 0);
            const TRAP::Math::Quat b = TRAP::Math::Quat(1, 0, 0, 0);
            const float c = TRAP::Math::Dot(a, b);
            error += TRAP::Math::Equal(c, 1.0f, 0.001f) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestEqual()
    {
        int32_t error = 0;

        const TRAP::Math::Quat q(1, 0, 0, 0);
        const TRAP::Math::Quat p(1, 0, 0, 0);
        error += TRAP::Math::All(TRAP::Math::Equal(q, p, TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestNotEqual()
    {
        int32_t error = 0;

        const TRAP::Math::Quat q(1, 0, 0, 0);
        const TRAP::Math::Quat p(1, 0, 0, 0);
        error += TRAP::Math::Any(TRAP::Math::NotEqual(q, p, TRAP::Math::Epsilon<float>())) ? 1 : 0;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestLookAt()
    {
        int32_t error = 0;

        TRAP::Math::Vec3 eye(0.0f);
        TRAP::Math::Vec3 center(1.1f, -2.0f, 3.1416f);
        TRAP::Math::Vec3 up(-0.17f, 7.23f, -1.744f);

        TRAP::Math::Quat testQuat = TRAP::Math::QuaternionLookAt(TRAP::Math::Normalize(center - eye), up); //BUG
        TRAP::Math::Quat testMat = TRAP::Math::Conjugate(TRAP::Math::QuaternionCast(TRAP::Math::LookAt(eye, center, up)));

        error += static_cast<int32_t>(TRAP::Math::Abs(TRAP::Math::Length(testQuat) - 1.0f) > TRAP::Math::Epsilon<float>());
        error += static_cast<int32_t>(TRAP::Math::Min(TRAP::Math::Length(testQuat + (-testMat)), TRAP::Math::Length(testQuat + testMat)) > TRAP::Math::Epsilon<float>());

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestAngle()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Quat q = TRAP::Math::Quat(TRAP::Math::Vec3(1, 0, 0), TRAP::Math::Vec3(0, 1, 0));
            const float a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            error += TRAP::Math::Equal(a, 90.0f, 0.001f) ? 0 : 1;
        }

        {
            const TRAP::Math::Quat q = TRAP::Math::Quat(TRAP::Math::Vec3(0, 1, 0), TRAP::Math::Vec3(1, 0, 0));
            const float a = TRAP::Math::Degrees(TRAP::Math::Angle(q));
            error += TRAP::Math::Equal(a, 90.0f, 0.001f) ? 0 : 1;
        }

        {
            const TRAP::Math::Quat q = TRAP::Math::AngleAxis(TRAP::Math::TwoPI<float>() - 1.0f, TRAP::Math::Vec3(1, 0, 0));
            const float a = TRAP::Math::Angle(q);
            error += TRAP::Math::Equal(a, 1.0f, 0.001f) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMul()
    {
        int32_t error = 0;

        TRAP::Math::Quat temp1 = TRAP::Math::Normalize(TRAP::Math::Quat(1.0f, TRAP::Math::Vec3(0.0, 1.0, 0.0)));
        TRAP::Math::Quat temp2 = TRAP::Math::Normalize(TRAP::Math::Quat(0.5f, TRAP::Math::Vec3(1.0, 0.0, 0.0)));

        TRAP::Math::Vec3 transformed0 = (temp1 * TRAP::Math::Vec3(0.0, 1.0, 0.0) * TRAP::Math::Inverse(temp1));
        [[maybe_unused]] TRAP::Math::Vec3 temp4 = temp2 * transformed0 * TRAP::Math::Inverse(temp2);

        TRAP::Math::Quat temp5 = TRAP::Math::Normalize(temp1 * temp2);
        [[maybe_unused]] TRAP::Math::Vec3 temp6 = temp5 * TRAP::Math::Vec3(0.0, 1.0, 0.0) * TRAP::Math::Inverse(temp5);

        TRAP::Math::Quat temp7(1.0f, TRAP::Math::Vec3(0.0, 1.0, 0.0));

        temp7 *= temp5;
        temp7 *= TRAP::Math::Inverse(temp5);

        error += TRAP::Math::Any(TRAP::Math::NotEqual(temp7, TRAP::Math::Quat(1.0f, TRAP::Math::Vec3(0.0, 1.0, 0.0)), TRAP::Math::Epsilon<float>())) ? 1 : 0;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestQuatMulVec()
    {
        int32_t error = 0;

        TRAP::Math::Quat q = TRAP::Math::AngleAxis(TRAP::Math::PI<float>() * 0.5f, TRAP::Math::Vec3(0, 0, 1));
        TRAP::Math::Vec3 v(1, 0, 0);
        TRAP::Math::Vec3 u(q * v);
        TRAP::Math::Vec3 w(u * q);

        error += TRAP::Math::All(TRAP::Math::Equal(v, w, 0.01f)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestQuatAngle()
    {
        int32_t error = 0;

        {
            TRAP::Math::Quat q = TRAP::Math::AngleAxis(TRAP::Math::PI<float>() * 0.25f, TRAP::Math::Vec3(0, 0, 1));
            TRAP::Math::Quat n = TRAP::Math::Normalize(q);
            float l = TRAP::Math::Length(n);
            error += TRAP::Math::Equal(l, 1.0f, 0.01f) ? 0 : 1;
            float a = TRAP::Math::Angle(n);
            error += TRAP::Math::Equal(a, TRAP::Math::PI<float>() * 0.25f, 0.01f) ? 0 : 1;
        }

        {
            TRAP::Math::Quat q = TRAP::Math::AngleAxis(TRAP::Math::PI<float>() * 0.25f, TRAP::Math::Normalize(TRAP::Math::Vec3(0, 1, 1)));
            TRAP::Math::Quat n = TRAP::Math::Normalize(q);
            float l = TRAP::Math::Length(n);
            error += TRAP::Math::Equal(l, 1.0f, 0.01f) ? 0 : 1;
            float a = TRAP::Math::Angle(n);
            error += TRAP::Math::Equal(a, TRAP::Math::PI<float>() * 0.25f, 0.01f) ? 0 : 1;
        }

        {
            TRAP::Math::Quat q = TRAP::Math::AngleAxis(TRAP::Math::PI<float>() * 0.25f, TRAP::Math::Normalize(TRAP::Math::Vec3(1, 2, 3)));
            TRAP::Math::Quat n = TRAP::Math::Normalize(q);
            float l = TRAP::Math::Length(n);
            error += TRAP::Math::Equal(l, 1.0f, 0.01f) ? 0 : 1;
            float a = TRAP::Math::Angle(n);
            error += TRAP::Math::Equal(a, TRAP::Math::PI<float>() * 0.25f, 0.01f) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestQuatAngleAxis()
    {
        int32_t error = 0;

        TRAP::Math::Quat a = TRAP::Math::AngleAxis(0.0f, TRAP::Math::Vec3(0.0f, 0.0f, 1.0f));
        TRAP::Math::Quat b = TRAP::Math::AngleAxis(TRAP::Math::PI<float>() * 0.5f, TRAP::Math::Vec3(0, 0, 1));
        TRAP::Math::Quat c = TRAP::Math::Mix(a, b, 0.5f);
        TRAP::Math::Quat d = TRAP::Math::AngleAxis(TRAP::Math::PI<float>() * 0.25f, TRAP::Math::Vec3(0, 0, 1));

        error += TRAP::Math::Equal(c.x, d.x, 0.01f) ? 0 : 1;
        error += TRAP::Math::Equal(c.y, d.y, 0.01f) ? 0 : 1;
        error += TRAP::Math::Equal(c.z, d.z, 0.01f) ? 0 : 1;
        error += TRAP::Math::Equal(c.w, d.w, 0.01f) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestQuatNormalize()
    {
        int32_t error = 0;

        {
            TRAP::Math::Quat q = TRAP::Math::AngleAxis(TRAP::Math::PI<float>() * 0.25f, TRAP::Math::Vec3(0, 0, 1));
            TRAP::Math::Quat n = TRAP::Math::Normalize(q);
            float l = TRAP::Math::Length(n);
            error += TRAP::Math::Equal(l,  1.0f, 0.000001f) ? 0 : 1;
        }

        {
            TRAP::Math::Quat q = TRAP::Math::AngleAxis(TRAP::Math::PI<float>() * 0.25f, TRAP::Math::Vec3(0, 0, 2));
            TRAP::Math::Quat n = TRAP::Math::Normalize(q);
            float l = TRAP::Math::Length(n);
            error += TRAP::Math::Equal(l,  1.0f, 0.000001f) ? 0 : 1;
        }

        {
            TRAP::Math::Quat q = TRAP::Math::AngleAxis(TRAP::Math::PI<float>() * 0.25f, TRAP::Math::Vec3(1, 2, 3));
            TRAP::Math::Quat n = TRAP::Math::Normalize(q);
            float l = TRAP::Math::Length(n);
            error += TRAP::Math::Equal(l,  1.0f, 0.000001f) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestQuatEuler()
    {
        int32_t error = 0;

        {
            TRAP::Math::Quat q(1.0f, 0.0f, 0.0f, 1.0f);
            float roll = TRAP::Math::Roll(q);
            float pitch = TRAP::Math::Pitch(q);
            float yaw = TRAP::Math::Yaw(q);
            TRAP::Math::Vec3 angles = TRAP::Math::EulerAngles(q);
            error += TRAP::Math::All(TRAP::Math::Equal(angles, TRAP::Math::Vec3(pitch, yaw, roll), 0.000001f)) ? 0 : 1;
        }

        {
            TRAP::Math::Quatd q(1.0, 0.0, 0.0, 1.0);
            double roll = TRAP::Math::Roll(q);
            double pitch = TRAP::Math::Pitch(q);
            double yaw = TRAP::Math::Yaw(q);
            TRAP::Math::Vec3d angles = TRAP::Math::EulerAngles(q);
            error += TRAP::Math::All(TRAP::Math::Equal(angles, TRAP::Math::Vec3d(pitch, yaw, roll), 0.000001)) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestQuatSLerp()
    {
        int32_t error = 0;

        const float epsilon = 0.0001f;

        float sqrt2 = TRAP::Math::Sqrt(2.0f) / 2.0f;
        TRAP::Math::Quat id(static_cast<float>(1), static_cast<float>(0), static_cast<float>(0), static_cast<float>(0));
        TRAP::Math::Quat y90Rot(sqrt2, 0.0f, sqrt2, 0.0f);
        [[maybe_unused]] TRAP::Math::Quat y180Rot(0.0f, 0.0f, 1.0f, 0.0f);

        TRAP::Math::Quat id2 = TRAP::Math::SLerp(id, y90Rot, 0.0f);
        error += TRAP::Math::All(TRAP::Math::Equal(id, id2, epsilon)) ? 0 : 1;

        TRAP::Math::Quat y90Rot2 = TRAP::Math::SLerp(id, y90Rot, 1.0f);
        error += TRAP::Math::All(TRAP::Math::Equal(y90Rot, y90Rot2, epsilon)) ? 0 : 1;

        [[maybe_unused]] TRAP::Math::Quat y45Rot1 = TRAP::Math::SLerp(id, y90Rot, 0.5f);

        TRAP::Math::Quat ym45Rot2 = TRAP::Math::SLerp(y90Rot, id, 0.5f);

        TRAP::Math::Quat y45Rot3 = TRAP::Math::SLerp(id, -y90Rot, 0.5f);
        float y45Angle3 = TRAP::Math::Angle(y45Rot3);
        error += TRAP::Math::Equal(y45Angle3, TRAP::Math::PI<float>() * 0.25f, epsilon) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(ym45Rot2, y45Rot3, epsilon)) ? 0 : 1;

        TRAP::Math::Quat y45Rot4 = TRAP::Math::SLerp(-y90Rot, id, 0.5f);
        error += TRAP::Math::All(TRAP::Math::Equal(ym45Rot2, -y45Rot4, epsilon)) ? 0 : 1;

        TRAP::Math::Quat y90Rot3 = TRAP::Math::SLerp(y90Rot, y90Rot, 0.5f);
        error += TRAP::Math::All(TRAP::Math::Equal(y90Rot, y90Rot3, epsilon)) ? 0 : 1;

        TRAP::Math::Quat xz90Rot = TRAP::Math::SLerp(id, -y90Rot, 0.5f);
        float xz90Angle = TRAP::Math::Angle(xz90Rot);
        error += TRAP::Math::Equal(xz90Angle, TRAP::Math::PI<float>() * 0.25f, epsilon) ? 0 : 1;

        [[maybe_unused]] TRAP::Math::Quat almostId = TRAP::Math::SLerp(id, TRAP::Math::AngleAxis(0.1f, TRAP::Math::Vec3(0.0f, 1.0f, 0.0f)), 0.5f);

        {
            TRAP::Math::Quat a(-1, 0, 0, 0);

            TRAP::Math::Quat result = TRAP::Math::SLerp(a, id, 0.5f);

            error += TRAP::Math::Equal(TRAP::Math::Pow(TRAP::Math::Dot(id, result), 2.0f), 1.0f, 0.01f) ? 0 : 1;
        }

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestQuatSLerpSpins()
    {
        int32_t error = 0;

        const float epsilon = 0.0001f;

        float sqrt2 = TRAP::Math::Sqrt(2.0f) / 2.0f;
        TRAP::Math::Quat id(static_cast<float>(1), static_cast<float>(0), static_cast<float>(0), static_cast<float>(0));
        TRAP::Math::Quat y90Rot(sqrt2, 0.0f, sqrt2, 0.0f);
        TRAP::Math::Quat y180Rot(0.0f, 0.0f, 1.0f, 0.0f);

        TRAP::Math::Quat id2 = TRAP::Math::SLerp(id, id, 1.0f, 1);
        error += TRAP::Math::All(TRAP::Math::Equal(id, id2, epsilon)) ? 0 : 1;

        TRAP::Math::Quat id3 = TRAP::Math::SLerp(id, id, 1.0f, 2);
        error += TRAP::Math::All(TRAP::Math::Equal(id, id3, epsilon)) ? 0 : 1;

        TRAP::Math::Quat y90Rot2 = TRAP::Math::SLerp(id, y90Rot, 1.0f, 1);
        error += TRAP::Math::All(TRAP::Math::Equal(y90Rot, -y90Rot2, epsilon)) ? 0 : 1;

        TRAP::Math::Quat y90Rot3 = TRAP::Math::SLerp(id, y90Rot, 8.0f / 9.0f, 2);
        error += TRAP::Math::All(TRAP::Math::Equal(id, y90Rot3, epsilon)) ? 0 : 1;

        TRAP::Math::Quat y90Rot4 = TRAP::Math::SLerp(id, y90Rot, 0.2f, 1);
        error += TRAP::Math::All(TRAP::Math::Equal(y90Rot, y90Rot4, epsilon)) ? 0 : 1;

        TRAP::Math::Quat ym45Rot2 = TRAP::Math::SLerp(y90Rot, id, 0.9f, 1);
        TRAP::Math::Quat ym45Rot3 = TRAP::Math::SLerp(y90Rot, id, 0.5f);
        error += TRAP::Math::All(TRAP::Math::Equal(-ym45Rot2, ym45Rot3, epsilon)) ? 0 : 1;

        TRAP::Math::Quat y45Rot3 = TRAP::Math::SLerp(id, -y90Rot, 0.5f, 0);
        float y45Angle3 = TRAP::Math::Angle(y45Rot3);
        error += TRAP::Math::Equal(y45Angle3, TRAP::Math::PI<float>() * 0.25f, epsilon) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(ym45Rot3, y45Rot3, epsilon)) ? 0 : 1;

        TRAP::Math::Quat y45Rot4 = TRAP::Math::SLerp(-y90Rot, id, 0.5f, 0);
        error += TRAP::Math::All(TRAP::Math::Equal(ym45Rot2, y45Rot4, epsilon)) ? 0 : 1;

        TRAP::Math::Quat y90Rot5 = TRAP::Math::SLerp(y90Rot, y90Rot, 0.5f, 2);
        error += TRAP::Math::All(TRAP::Math::Equal(y90Rot, y90Rot5, epsilon)) ? 0 : 1;

        TRAP::Math::Quat xz90Rot = TRAP::Math::SLerp(id, -y90Rot, 0.5f, 1);
        float xz90Angle = TRAP::Math::Angle(xz90Rot);
        error += TRAP::Math::Equal(xz90Angle, TRAP::Math::PI<float>() * 1.25f, epsilon) ? 0 : 1;

        TRAP::Math::Quat neg90Rot = TRAP::Math::SLerp(id, y90Rot, 2.0f / 3.0f, -1);
        error += TRAP::Math::All(TRAP::Math::Equal(y180Rot, -neg90Rot, epsilon)) ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestToString()
    {
        int32_t error = 0;

        TRAP::Math::Quat a1 = TRAP::Math::Quat(1.0f, 2.0f, 3.0f, 4.0f);
        std::string a2 = a1.ToString();
        error += a2 != std::string("Quatf(1.000000, {2.000000, 3.000000, 4.000000})") ? 1 : 0;

        TRAP::Math::Quatd b1 = TRAP::Math::Quatd(1.0, 2.0, 3.0, 4.0);
        std::string b2 = b1.ToString();
        error += b2 != std::string("Quatd(1.000000, {2.000000, 3.000000, 4.000000})") ? 1 : 0;

        return error;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunTypeQuaternionTests()
{
    int32_t error = 0;

    error += TypeQuaternion::TestConstructor();
    error += TypeQuaternion::TestTwoAxisConstructor();
    error += TypeQuaternion::TestSize();
    TypeQuaternion::TestConstexpr();

    error += TypeQuaternion::TestConjugate();
    error += TypeQuaternion::TestMix();

    error += TypeQuaternion::TestLog<TRAP::Math::Quat, TRAP::Math::Vec3>();
    error += TypeQuaternion::TestLog<TRAP::Math::Quatd, TRAP::Math::Vec3d>();

    error += TypeQuaternion::TestPow<TRAP::Math::Quat, TRAP::Math::Vec3>();
    error += TypeQuaternion::TestPow<TRAP::Math::Quatd, TRAP::Math::Vec3d>();

    error += TypeQuaternion::TestLength();
    error += TypeQuaternion::TestNormalize();
    error += TypeQuaternion::TestDot();

    error += TypeQuaternion::TestEqual();
    error += TypeQuaternion::TestNotEqual();

    error += TypeQuaternion::TestLookAt();

    error += TypeQuaternion::TestAngle();

    error += TypeQuaternion::TestMul();
    error += TypeQuaternion::TestQuatMulVec();
    error += TypeQuaternion::TestQuatAngle();
    error += TypeQuaternion::TestQuatAngleAxis();
    error += TypeQuaternion::TestQuatNormalize();
    error += TypeQuaternion::TestQuatEuler();
    error += TypeQuaternion::TestQuatSLerp();
    error += TypeQuaternion::TestQuatSLerpSpins();

    error += TypeQuaternion::TestToString();

    return error;
}
