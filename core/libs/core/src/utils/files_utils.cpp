#include "utils/files_utils.hpp"

#include <fstream>
#include <unistd.h>
#include <limits.h>

namespace aardvark::utils {

namespace fs = std::experimental::filesystem;

std::string get_self_path() {
    char buffer[PATH_MAX];
    auto len = ::readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    buffer[len] = '\0';
    auto path = std::string(buffer);
    return path.substr(0, path.rfind('/'));
}

std::string read_text_file(fs::path path) {
    auto size = fs::file_size(path);
    auto result = std::string(size, ' ');
    auto stream = std::ifstream(path.string());
    stream.read(result.data(), size);
    return result;
}

}  // namespace aardvark::utils
