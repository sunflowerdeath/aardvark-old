#include "../js/bindings_host.hpp"
#include "../utils/files_utils.hpp"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

int main() {
    auto host = aardvark::js::BindingsHost();

    auto src_path = fs::path(aardvark::utils::get_self_path())
                        .append("../src/examples/src.js");
    auto src_str = aardvark::utils::read_text_file(src_path);
    auto src = JSStringCreateWithUTF8CString(src_str.c_str());

    auto exception = JSValueRef();
    auto result = JSEvaluateScript(host.ctx,   // ctx,
                                   src,        // script
                                   nullptr,    // thisObject,
                                   nullptr,    // sourceURL,
                                   1,          // startingLineNumber,
                                   &exception  // exception
    );
    auto str = JSValueToStringCopy(host.ctx, exception, nullptr);
    std::cout << aardvark::js::jsstring_to_std(str) << std::endl;
    JSStringRelease(str);
};
