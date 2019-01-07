#include "test/gtest/gtest.h"

#include <fstream>

#include <vector>

#include <algorithm>
#include <chrono>
#include <utility>
#include <cassert>
#include <filesystem>

TEST(correctness_trigram, small_file_indexing)
{
    std::string directory = "./tmp/";
    fs::create_directory(directory);

    fs::remove_all(directory);
}

TEST(correctness_trigram, large_file_indexing)
{
    std::string directory = "./tmp/";
    fs::create_directory(directory);

    fs::remove_all(directory);
}
