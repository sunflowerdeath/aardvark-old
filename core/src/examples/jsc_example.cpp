#include <iostream>
#include <experimental/filesystem>
#include <chrono>

#include "../js/bindings_host.hpp"
#include "../js/helpers.hpp"
#include "../utils/files_utils.hpp"

namespace fs = std::experimental::filesystem;

void run(const fs::path& filepath) {
    auto host = aardvark::js::BindingsHost();
    auto src = aardvark::utils::read_text_file(filepath);
    std::cout << "Run script: " << filepath << std::endl; 
    host.eval_script(src);
    host.app->run();
    host.event_loop->run();
}

int main(int argc, char *argv[]) {
    auto filepath = fs::current_path().append(argv[1]);
    while (true) {
        run(filepath);
        std::cout << "\n--- Restart ---\n" << std:: endl;
    }
};
