#include <iostream>
#include <string>
#include "spdlog/spdlog.h"
#include "../utils/log.hpp"
#include "../js/bindings_host.hpp"
#include "../js/helpers.hpp"

void run(const std::string& filepath) {
    auto host = aardvark::js::BindingsHost();
    // auto src = aardvark::utils::read_text_file(filepath);
    host.module_loader->load_from_file(filepath);
    host.run();
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);
    // auto filepath = fs::current_path().append();
    while (true) {
        run(argv[1]);
        aardvark::Log::info("Press 'R' to restart...");
        char ch;
        do {
            std::cin >> ch;
            if (ch == 'r' || ch == 'R') break;
        } while (ch != EOF);
        aardvark::Log::info("Restart");
    }
}
