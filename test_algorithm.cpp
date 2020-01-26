#include <gtest/gtest.h>
#include "algorithm.h"

using namespace Cascade;

TEST (Algorithm, get_by_name_does_not_exist)
{
    EXPECT_EQ(Algorithm::get_by_name("nonsense"), nullptr);
}

TEST (Algorithm, original)
{
    Algorithm* algorithm = Algorithm::get_by_name("original");
    EXPECT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->name, "original");
    EXPECT_EQ(algorithm->cascade_iterations, 4u);
    EXPECT_EQ(algorithm->block_size_function(1, 0.0), 73000u);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1), 8u);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01), 73u);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01), 146u);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01), 292u);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001), 730u);
    EXPECT_EQ(algorithm->biconf_iterations, 0u);
    EXPECT_FALSE(algorithm->biconf_error_free_streak);
    EXPECT_FALSE(algorithm->biconf_correct_complement);
    EXPECT_FALSE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->sub_block_reuse);
    EXPECT_FALSE(algorithm->block_parity_inference);
}
