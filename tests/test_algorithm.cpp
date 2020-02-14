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
    EXPECT_EQ(algorithm->nr_cascade_iterations, 4);
    EXPECT_EQ(algorithm->block_size_function(1, 0.0, 1000), 73000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1, 1000), 8);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01, 1000), 73);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01, 1000), 146);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01, 1000), 292);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001, 1000), 730);
    EXPECT_EQ(algorithm->nr_biconf_iterations, 0);
    EXPECT_FALSE(algorithm->biconf_error_free_streak);
    EXPECT_FALSE(algorithm->biconf_correct_complement);
    EXPECT_FALSE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->sub_block_reuse);
    EXPECT_FALSE(algorithm->block_parity_inference);
}
