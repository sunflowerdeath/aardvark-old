#pragma once

#include <experimental/filesystem>
#include "SkImage.h"
#include "SkData.h"

namespace aardvark::utils {

std::string get_self_path();

std::string read_text_file(std::experimental::filesystem::path path);

}  // namespace aardvark::utils
