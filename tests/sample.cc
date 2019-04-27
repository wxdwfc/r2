#include <gtest/gtest.h>

#include "test_ud.hpp"

TEST(FactorialTest, MagicNumber) {
  EXPECT_EQ(72 + 1, 73);
}

TEST(FactorialTest_1, MagicNumber) {
  EXPECT_EQ(72 + 1, 72);
}

TEST(FactorialTest_1, MagicNumber3) {
  EXPECT_EQ(72 + 1, 72);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
