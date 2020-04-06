#include <gtest/gtest.h>
#include "key.h"
#include "random.h"

using namespace Cascade;

TEST (Key, random_constructor)
{
    set_random_uint64_seed(1111);

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
    set_random_uint64_seed(1111);

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
