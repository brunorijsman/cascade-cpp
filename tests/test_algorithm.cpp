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
    EXPECT_EQ(algorithm->block_size_function(1, 0.0, 10000), 73000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1, 10000), 8);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01, 10000), 73);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01, 10000), 146);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01, 10000), 292);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001, 10000), 730);
    EXPECT_EQ(algorithm->nr_biconf_iterations, 0);
    EXPECT_FALSE(algorithm->biconf_error_free_streak);
    EXPECT_FALSE(algorithm->biconf_correct_complement);
    EXPECT_FALSE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->block_parity_inference);
}

TEST (Algorithm, biconf)
{
    Algorithm* algorithm = Algorithm::get_by_name("biconf");
    EXPECT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->name, "biconf");
    EXPECT_EQ(algorithm->nr_cascade_iterations, 2);
    EXPECT_EQ(algorithm->block_size_function(1, 0.0, 10000), 92000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1, 10000), 10);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01, 10000), 92);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01, 10000), 276);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01, 10000), 828);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001, 10000), 920);
    EXPECT_EQ(algorithm->nr_biconf_iterations, 10);
    EXPECT_TRUE(algorithm->biconf_error_free_streak);
    EXPECT_FALSE(algorithm->biconf_correct_complement);
    EXPECT_FALSE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->block_parity_inference);
}

TEST (Algorithm, biconf_cascade)
{
    Algorithm* algorithm = Algorithm::get_by_name("biconf-cascade");
    EXPECT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->name, "biconf-cascade");
    EXPECT_EQ(algorithm->nr_cascade_iterations, 2);
    EXPECT_EQ(algorithm->block_size_function(1, 0.0, 10000), 92000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1, 10000), 10);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01, 10000), 92);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01, 10000), 276);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01, 10000), 828);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001, 10000), 920);
    EXPECT_EQ(algorithm->nr_biconf_iterations, 10);
    EXPECT_TRUE(algorithm->biconf_error_free_streak);
    EXPECT_FALSE(algorithm->biconf_correct_complement);
    EXPECT_TRUE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->block_parity_inference);
}

TEST (Algorithm, biconf_complement)
{
    Algorithm* algorithm = Algorithm::get_by_name("biconf-complement");
    EXPECT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->name, "biconf-complement");
    EXPECT_EQ(algorithm->nr_cascade_iterations, 2);
    EXPECT_EQ(algorithm->block_size_function(1, 0.0, 10000), 92000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1, 10000), 10);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01, 10000), 92);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01, 10000), 276);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01, 10000), 828);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001, 10000), 920);
    EXPECT_EQ(algorithm->nr_biconf_iterations, 10);
    EXPECT_TRUE(algorithm->biconf_error_free_streak);
    EXPECT_TRUE(algorithm->biconf_correct_complement);
    EXPECT_FALSE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->block_parity_inference);
}

TEST (Algorithm, yanetal)
{
    Algorithm* algorithm = Algorithm::get_by_name("yanetal");
    EXPECT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->name, "yanetal");
    EXPECT_EQ(algorithm->nr_cascade_iterations, 10);
    EXPECT_EQ(algorithm->block_size_function(1, 0.0, 10000), 80000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1, 10000), 8);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01, 10000), 80);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01, 10000), 400);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01, 10000), 5000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001, 10000), 800);
    EXPECT_EQ(algorithm->nr_biconf_iterations, 0);
    EXPECT_FALSE(algorithm->biconf_error_free_streak);
    EXPECT_FALSE(algorithm->biconf_correct_complement);
    EXPECT_FALSE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->block_parity_inference);
}

TEST (Algorithm, option3)
{
    Algorithm* algorithm = Algorithm::get_by_name("option3");
    EXPECT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->name, "option3");
    EXPECT_EQ(algorithm->nr_cascade_iterations, 16);
    EXPECT_EQ(algorithm->block_size_function(1, 0.0, 10000), 100000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1, 10000), 10);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01, 10000), 100);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01, 10000), 200);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01, 10000), 5000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001, 10000), 1000);
    EXPECT_EQ(algorithm->nr_biconf_iterations, 0);
    EXPECT_FALSE(algorithm->biconf_error_free_streak);
    EXPECT_FALSE(algorithm->biconf_correct_complement);
    EXPECT_FALSE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->block_parity_inference);
}

TEST (Algorithm, option4)
{
    Algorithm* algorithm = Algorithm::get_by_name("option4");
    EXPECT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->name, "option4");
    EXPECT_EQ(algorithm->nr_cascade_iterations, 16);
    EXPECT_EQ(algorithm->block_size_function(1, 0.0, 10000), 100000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1, 10000), 10);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01, 10000), 100);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01, 10000), 200);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01, 10000), 5000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001, 10000), 1000);
    EXPECT_EQ(algorithm->nr_biconf_iterations, 0);
    EXPECT_FALSE(algorithm->biconf_error_free_streak);
    EXPECT_FALSE(algorithm->biconf_correct_complement);
    EXPECT_FALSE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->block_parity_inference);
}

TEST (Algorithm, option7)
{
    Algorithm* algorithm = Algorithm::get_by_name("option7");
    EXPECT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->name, "option7");
    EXPECT_EQ(algorithm->nr_cascade_iterations, 14);
    EXPECT_EQ(algorithm->block_size_function(1, 0.0, 10000), 131072);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1, 10000), 16);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01, 10000), 128);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01, 10000), 512);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01, 10000), 5000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001, 10000), 1024);
    EXPECT_EQ(algorithm->nr_biconf_iterations, 0);
    EXPECT_FALSE(algorithm->biconf_error_free_streak);
    EXPECT_FALSE(algorithm->biconf_correct_complement);
    EXPECT_FALSE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->block_parity_inference);
}

TEST (Algorithm, option8)
{
    Algorithm* algorithm = Algorithm::get_by_name("option8");
    EXPECT_NE(algorithm, nullptr);
    EXPECT_EQ(algorithm->name, "option8");
    EXPECT_EQ(algorithm->nr_cascade_iterations, 14);
    EXPECT_EQ(algorithm->block_size_function(1, 0.0, 10000), 131072);
    EXPECT_EQ(algorithm->block_size_function(1, 0.1, 10000), 8);
    EXPECT_EQ(algorithm->block_size_function(1, 0.01, 10000), 128);
    EXPECT_EQ(algorithm->block_size_function(2, 0.01, 10000), 1024);
    EXPECT_EQ(algorithm->block_size_function(3, 0.01, 10000), 4096);
    EXPECT_EQ(algorithm->block_size_function(4, 0.01, 10000), 5000);
    EXPECT_EQ(algorithm->block_size_function(1, 0.001, 10000), 1024);
    EXPECT_EQ(algorithm->nr_biconf_iterations, 0);
    EXPECT_FALSE(algorithm->biconf_error_free_streak);
    EXPECT_FALSE(algorithm->biconf_correct_complement);
    EXPECT_FALSE(algorithm->biconf_cascade);
    EXPECT_FALSE(algorithm->block_parity_inference);
}

TEST (Algorithm, get_all_algorithm_names)
{
    auto algorithm_names = Algorithm::get_all_algorithm_names();
    EXPECT_EQ(algorithm_names.size(), 9U);
    EXPECT_TRUE(std::find(algorithm_names.begin(), algorithm_names.end(), "original") !=
                algorithm_names.end());
    EXPECT_TRUE(std::find(algorithm_names.begin(), algorithm_names.end(), "biconf") !=
                algorithm_names.end());
    EXPECT_TRUE(std::find(algorithm_names.begin(), algorithm_names.end(), "biconf-cascade") !=
                algorithm_names.end());
    EXPECT_TRUE(std::find(algorithm_names.begin(), algorithm_names.end(), "biconf-complement") !=
                algorithm_names.end());
    EXPECT_TRUE(std::find(algorithm_names.begin(), algorithm_names.end(), "yanetal") !=
                algorithm_names.end());
    EXPECT_TRUE(std::find(algorithm_names.begin(), algorithm_names.end(), "option3") !=
                algorithm_names.end());
    EXPECT_TRUE(std::find(algorithm_names.begin(), algorithm_names.end(), "option4") !=
                algorithm_names.end());
    EXPECT_TRUE(std::find(algorithm_names.begin(), algorithm_names.end(), "option7") !=
                algorithm_names.end());
    EXPECT_TRUE(std::find(algorithm_names.begin(), algorithm_names.end(), "option8") !=
                algorithm_names.end());
}
