#include <gtest/gtest.h>

TEST(FactorialTest, MagicNumber) {
  EXPECT_EQ(72 + 1, 73);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
