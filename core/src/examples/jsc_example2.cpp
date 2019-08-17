#include <experimental/filesystem>
#include "../js/bindings_host.hpp"
#include "../utils/files_utils.hpp"

namespace fs = std::experimental::filesystem;

int main() {
    auto host = aardvark::js::BindingsHost();

    auto src_path = fs::current_path().append("src.js");
    auto src = aardvark::utils::read_text_file(src_path);

    auto exception = JSValueRef();
    auto result = host.eval_script(src, &exception);
    auto exception_str = JSValueToStringCopy(host.ctx, exception, nullptr);
    std::cout << aardvark::js::jsstring_to_std(exception_str) << std::endl;
    host.event_loop->run();
};
