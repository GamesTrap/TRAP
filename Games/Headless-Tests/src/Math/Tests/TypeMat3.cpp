#include "TypeMat3.h"

#include <TRAP.h>

namespace TypeMat3
{
    template<typename genType>
    int32_t EntryCast()
    {
        int32_t error = 0;

        genType a(1.0f);
        TRAP::Math::Mat3 b(a);
        TRAP::Math::Mat3 identity(1.0f);

        error += TRAP::Math::All(TRAP::Math::Equal(b, identity, TRAP::Math::Epsilon<float>())) ? 0 : 1;

        return error;
    }

    int32_t TestCast()
    {
        int32_t error = 0;

        error += EntryCast<TRAP::Math::Mat3>();
        error += EntryCast<TRAP::Math::Mat4>();

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestConstructor()
    {
        int32_t error = 0;

        TRAP::Math::Mat3 m0(TRAP::Math::Vec3(0, 1, 2), TRAP::Math::Vec3(3, 4, 5), TRAP::Math::Vec3(6, 7, 8));

        TRAP::Math::Mat3 m1{0, 1, 2, 3, 4, 5, 6, 7, 8};

        TRAP::Math::Mat3 m2{{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};

        error += TRAP::Math::All(TRAP::Math::Equal(m0, m2, TRAP::Math::Epsilon<float>())) ? 0 : 1;
        error += TRAP::Math::All(TRAP::Math::Equal(m1, m2, TRAP::Math::Epsilon<float>())) ? 0 : 1;

        std::vector<TRAP::Math::Mat3> v1
        {
            {0, 1, 2, 3, 4, 5, 6, 7, 8},
            {0, 1, 2, 3, 4, 5, 6, 7, 8}
        };

        std::vector<TRAP::Math::Mat3> v2
        {
            {
                {0, 1, 2},
                {3, 4, 5},
                {6, 7, 8}
            },
            {
                {0, 1, 2},
                {3, 4, 5},
                {6, 7, 8}
            }
        };

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMat3()
    {
        TRAP::Math::Mat3d m(TRAP::Math::Vec3d(0.6f, 0.2f, 0.3f), TRAP::Math::Vec3d(0.2f, 0.7f, 0.5f), TRAP::Math::Vec3d(0.3f, 0.5f, 0.7f));
        TRAP::Math::Mat3d inv = TRAP::Math::Inverse(m);
        TRAP::Math::Mat3d res = m * inv;

        return TRAP::Math::All(TRAP::Math::Equal(res, TRAP::Math::Mat3d(1.0), 0.01)) ? 0 : 1;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestOperators()
    {
        TRAP::Math::Mat3 l(1.0f);
        TRAP::Math::Mat3 m(1.0f);
        TRAP::Math::Vec3 u(1.0f);
        TRAP::Math::Vec3 v(1.0f);
        float x = 1.0f;
        TRAP::Math::Vec3 a = m * u;
        TRAP::Math::Vec3 b = v * m;
        TRAP::Math::Mat3 n = x / m;
        TRAP::Math::Mat3 o = m / x;
        TRAP::Math::Mat3 p = x * m;
        TRAP::Math::Mat3 q = m * x;
        bool r = TRAP::Math::Any(TRAP::Math::NotEqual(m, q, TRAP::Math::Epsilon<float>()));
        bool s = TRAP::Math::All(TRAP::Math::Equal(m, l, TRAP::Math::Epsilon<float>()));

        return (s && !r) ? 0 : 1;
    }

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestMat3Inverse()
    {
        int32_t error = 0;

        {
            const TRAP::Math::Mat3 m(TRAP::Math::Vec3(0.6f, 0.2f, 0.3f), TRAP::Math::Vec3(0.2f, 0.7f, 0.5f), TRAP::Math::Vec3(0.3f, 0.5f, 0.7f));
            const TRAP::Math::Mat3 inverse = TRAP::Math::Inverse(m);
            const TRAP::Math::Mat3 identity = m * inverse;

            error += TRAP::Math::All(TRAP::Math::Equal(identity[0], TRAP::Math::Vec3(1.0f, 0.0f, 0.0f), TRAP::Math::Vec3(0.01f))) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(identity[1], TRAP::Math::Vec3(0.0f, 1.0f, 0.0f), TRAP::Math::Vec3(0.01f))) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(identity[2], TRAP::Math::Vec3(0.0f, 0.0f, 1.0f), TRAP::Math::Vec3(0.01f))) ? 0 : 1;
        }

        {
            const TRAP::Math::Mat3 m(TRAP::Math::Vec3(0.6f, 0.2f, 0.3f), TRAP::Math::Vec3(0.2f, 0.7f, 0.5f), TRAP::Math::Vec3(0.3f, 0.5f, 0.7f));
            const TRAP::Math::Mat3 identity = m / m;

            error += TRAP::Math::All(TRAP::Math::Equal(identity[0], TRAP::Math::Vec3(1.0f, 0.0f, 0.0f), TRAP::Math::Vec3(0.01f))) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(identity[1], TRAP::Math::Vec3(0.0f, 1.0f, 0.0f), TRAP::Math::Vec3(0.01f))) ? 0 : 1;
            error += TRAP::Math::All(TRAP::Math::Equal(identity[2], TRAP::Math::Vec3(0.0f, 0.0f, 1.0f), TRAP::Math::Vec3(0.01f))) ? 0 : 1;
        }

        return error;
}

//-------------------------------------------------------------------------------------------------------------------//

    int32_t TestSize()
    {
        int32_t error = 0;

        error += 36 == sizeof(TRAP::Math::Mat3) ? 0 : 1;
        error += 72 == sizeof(TRAP::Math::Mat3d) ? 0 : 1;
        error += TRAP::Math::Mat3().Length() == 3 ? 0 : 1;
        error += TRAP::Math::Mat3d().Length() == 3 ? 0 : 1;
        error += TRAP::Math::Mat3::Length() == 3 ? 0 : 1;
        error += TRAP::Math::Mat3d::Length() == 3 ? 0 : 1;

        return error;
    }

//-------------------------------------------------------------------------------------------------------------------//

    void TestMat3Constexpr()
    {
        static_assert(TRAP::Math::Mat3::Length() == 3, "Math: Failed constexpr");

        static constexpr TRAP::Math::Mat3 z(0.0f);
    }
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t RunTypeMat3Tests()
{
    int32_t error = 0;

    error += TypeMat3::TestCast();
    error += TypeMat3::TestConstructor();
    error += TypeMat3::TestMat3();
    error += TypeMat3::TestOperators();
    error += TypeMat3::TestMat3Inverse();
    error += TypeMat3::TestSize();
    TypeMat3::TestMat3Constexpr();

    return error;
}