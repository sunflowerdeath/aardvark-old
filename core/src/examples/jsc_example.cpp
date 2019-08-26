#include <iostream>
#include <experimental/filesystem>
#include <chrono>

#include "../js/bindings_host.hpp"
#include "../js/helpers.hpp"
#include "../utils/files_utils.hpp"

namespace fs = std::experimental::filesystem;

int main(int argc, char *argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  start.time_since_epoch())
                  .count();
    std::cout << ms << std::endl;
    auto host = aardvark::js::BindingsHost();

    auto src_path = fs::current_path().append(argv[1]);
    std::cout << "Open file: " << src_path << std::endl; 
    auto src = aardvark::utils::read_text_file(src_path);

    auto exception = JSValueRef();
    auto result = host.eval_script(src, &exception);
    if (exception != nullptr) {
        auto exception_str = JSValueToStringCopy(host.ctx, exception, nullptr);
        std::cout << aardvark::js::str_from_js(exception_str) << std::endl;

        auto obj = JSValueToObject(host.ctx, exception, nullptr);
        auto js_src = JSStringCreateWithUTF8CString(
            "if (this.line !== undefined) log('Error at line ' + this.line + ', column ' + this.column)");
        auto result = JSEvaluateScript(host.ctx,   // ctx,
                                       js_src,     // script
                                       obj,        // thisObject,
                                       nullptr,    // sourceURL,
                                       1,          // startingLineNumber,
                                       &exception  // exception
        );
        JSStringRelease(js_src);
    }
    host.event_loop->run();
};
