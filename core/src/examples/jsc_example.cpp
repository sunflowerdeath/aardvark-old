#include <iostream>
#include <string>

#include "spdlog/spdlog.h"
#include "../js/bindings_host.hpp"
#include "../js/helpers.hpp"

void run(const std::string& filepath) {
    auto host = aardvark::js::BindingsHost();
    // auto src = aardvark::utils::read_text_file(filepath);
    host.module_loader->load_from_file(filepath);
    host.run();
}

int main(int argc, char *argv[]) {
    spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
    // auto filepath = fs::current_path().append();
    while (true) {
        run(argv[1]);
        std::cout << "\n--- Restart ---\n" << std:: endl;
    }
}
