#include <gtest/gtest.h>
#include "key.h"

TEST (Key, constructor_random)
{
    Key::set_seed(1111);
    Key key_1(1);
    EXPECT_EQ(key_1.to_string(), "0");
    Key key_2(2);
    EXPECT_EQ(key_2.to_string(), "10");
    Key key_63(63);
    EXPECT_EQ(key_63.to_string(),
              "110000111001100100100010101000101100001110011001001000101010001");
    Key key_64(64);
    EXPECT_EQ(key_64.to_string(),
              "1011101111110001110100001001100010111011111100011101000010011000");
    Key key_65(65);
    EXPECT_EQ(key_65.to_string(),
              "10100110110100000000001100010100101001101101000000000011000101000");
    Key key_10_000(10000);
    Key key_100_000(100000);
}

TEST (Key, parity)
{
    Key::set_seed(2222);
    Key key_30(30);
    EXPECT_EQ(key_30.to_string(), "10");


    // Key key_150(65);
    // EXPECT_EQ(key_150.to_string(),
    //           "10100110110100000000001100010100101001101101000000000011000101000");
    
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
