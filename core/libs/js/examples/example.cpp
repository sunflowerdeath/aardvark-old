#include <iostream>
#include <string>
// #include "spdlog/spdlog.h"
// #include "../utils/log.hpp"
#include <aardvark_js/host.hpp>

void run(const std::string& filepath) {
    auto host = aardvark::js::Host();
    host.module_loader->load_from_file(filepath);
    host.run();
}

int main(int argc, char *argv[]) {
    // spdlog::set_level(spdlog::level::debug);
    run(argv[1]);
    /*
    // auto filepath = fs::current_path().append();
    while (true) {
        run(argv[1]);
        aardvark::Log::info("Enter 'r' to restart application");
        char ch;
        do {
            ch = std::cin.get();
            if (ch == 'r' || ch == 'R') break;
        } while (true);
        aardvark::Log::info("Restart");
    }
    */
}
