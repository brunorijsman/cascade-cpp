#include <gtest/gtest.h>
#include "key.h"
#include "random.h"

using namespace Cascade;

TEST (Key, random_constructor)
{
    random_seed(1111);

    Key key_1(1);
    EXPECT_EQ(key_1.to_string(), "1");

    Key key_2(2);
    EXPECT_EQ(key_2.to_string(), "10");

    Key key_63(63);
    EXPECT_EQ(key_63.to_string(),
              "010001010100010010011001110000100011000110110110001011111110101");

    Key key_64(64);
    EXPECT_EQ(key_64.to_string(),
              "0001100100001011100011111101110100101101010101101101111011110010");

    Key key_65(65);
    EXPECT_EQ(key_65.to_string(),
              "00101000110000000000101101100100011010011110110111000001000000000");

    Key key_66(66);
    EXPECT_EQ(key_66.to_string(),
              "011001111010111111101101010101100001000011010111111110110011110001");

    Key key_10_000(10000);

    Key key_100_000(100000);
}

TEST (Key, copy_constructor)
{
    random_seed(1111);

    Key key(80);
    EXPECT_EQ(key.to_string(),
              "11101100100000111010100001110110001110011001011110101110000110001000101010110101");

    Key key_copy(key);
    EXPECT_EQ(key_copy.to_string(),
              "11101100100000111010100001110110001110011001011110101110000110001000101010110101");

    // Make sure that changing a bit in the original key does not affect the copied key,
    // and vice versa.
    key.set_bit(78, 1);
    key_copy.set_bit(77, 0);
    EXPECT_EQ(key.to_string(),
              "11101100100000111010100001110110001110011001011110101110000110001000101010110111");
    EXPECT_EQ(key_copy.to_string(),
              "11101100100000111010100001110110001110011001011110101110000110001000101010110001");
}

TEST (Key, compute_range_parity)
{
    random_seed(2222);

    Key key_1(1);
    EXPECT_EQ(key_1.to_string(), "1");
    EXPECT_TRUE(key_1.compute_range_parity(0, 0));

    Key key_2(2);
    EXPECT_EQ(key_2.to_string(), "01");
    EXPECT_FALSE(key_2.compute_range_parity(0, 0));
    EXPECT_TRUE(key_2.compute_range_parity(1, 1));
    EXPECT_TRUE(key_2.compute_range_parity(0, 1));

    Key key_50(50);
    EXPECT_EQ(key_50.to_string(), "00010101010110000001010100111001000110010111100101");
    // 22 one bits, even parity
    EXPECT_FALSE(key_50.compute_range_parity(0, 49));
    // Range 2-12 = "11010101010": 6 one bits, even parity
    EXPECT_FALSE(key_50.compute_range_parity(2, 12));
    // Range 7-19 = "1000000110101": 5 one bits, odd parity 
    EXPECT_TRUE(key_50.compute_range_parity(7, 19));

    Key key_70(70);
    EXPECT_EQ(key_70.to_string(),
              "0010011001010010010000010101010100000100000101010000010001011101111111");
    // 28 one bits, even parity
    EXPECT_FALSE(key_70.compute_range_parity(0, 69));
    // Range 62-67 = "111110": 5 one bits, odd parity
    EXPECT_TRUE(key_70.compute_range_parity(62, 67));
}
