#include <gtest/gtest.h>

TEST (Key, Dummy) { 
    EXPECT_EQ (2 * 10, 20);
    EXPECT_EQ (2 * 7, 14);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
