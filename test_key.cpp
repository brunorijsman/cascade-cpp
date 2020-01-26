#include <gtest/gtest.h>
#include "key.h"

TEST (Key, constructor_random)
{
    Key::set_seed(1111);

    Key key_1(1);
    EXPECT_EQ(key_1.to_string(), "1");

    Key key_2(2);
    EXPECT_EQ(key_2.to_string(), "01");

    Key key_63(63);
    EXPECT_EQ(key_63.to_string(),
              "101011111110100011011011000110001000011100110010010001010100010");

    Key key_64(64);
    EXPECT_EQ(key_64.to_string(),
              "0100111101111011011010101011010010111011111100011101000010011000");

    Key key_65(65);
    EXPECT_EQ(key_65.to_string(),
              "00000000010000011101101111001011000100110110100000000001100010100");

    Key key_66(66);
    EXPECT_EQ(key_66.to_string(),
              "100011110011011111111010110000100001101010101101111111010111100110");

    Key key_10_000(10000);

    Key key_100_000(100000);
}

TEST (Key, compute_range_parity)
{
    Key::set_seed(2222);

    Key key_1(1);
    EXPECT_EQ(key_1.to_string(), "1");
    EXPECT_TRUE(key_1.compute_range_parity(0, 0));

    Key key_2(2);
    EXPECT_EQ(key_2.to_string(), "10");
    EXPECT_FALSE(key_2.compute_range_parity(0, 0));
    EXPECT_TRUE(key_2.compute_range_parity(1, 1));
    EXPECT_TRUE(key_2.compute_range_parity(0, 1));

    Key key_50(50);
    EXPECT_EQ(key_50.to_string(), "10100111101001100010011100101010000001101010101000");
    // 22 one bits, even parity
    EXPECT_FALSE(key_50.compute_range_parity(0, 49));
    // Range 2-12 = "11010101010": 6 one bits, even parity
    EXPECT_FALSE(key_50.compute_range_parity(2, 12));
    // Range 7-19 = "1000000110101": 5 one bits, odd parity 
    EXPECT_TRUE(key_50.compute_range_parity(7, 19));

    Key key_70(70);
    EXPECT_EQ(key_70.to_string(),
              "1111111011101000100000101010000010000010101010100000100100101001100100");
    // 28 one bits, even parity
    EXPECT_FALSE(key_70.compute_range_parity(0, 69));
    // Range 62-67 = "111110": 5 one bits, odd parity
    EXPECT_TRUE(key_70.compute_range_parity(62, 67));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
