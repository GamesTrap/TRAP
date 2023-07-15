#include <cstdint>

#include <gtest/gtest.h>

TEST(HelloTest, BasicAssertions)
{
    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7 * 6, 42);
}

int32_t main(int32_t argc, char** const argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
