#pragma once

#include <experimental/filesystem>

namespace aardvark::utils {

std::string get_self_path();

// TODO rename to read_file
std::string read_text_file(std::experimental::filesystem::path path);

}  // namespace aardvark::utils
