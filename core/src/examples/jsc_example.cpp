#include <iostream>
#include <string>

#include "../js/bindings_host.hpp"
#include "../js/helpers.hpp"

void run(const std::string& filepath) {
    auto host = aardvark::js::BindingsHost();
    // auto src = aardvark::utils::read_text_file(filepath);
    host.module_loader->load_from_file(filepath);
    host.run();
}

int main(int argc, char *argv[]) {
    // auto filepath = fs::current_path().append();
    while (true) {
        run(argv[1]);
        std::cout << "\n--- Restart ---\n" << std:: endl;
    }
};
