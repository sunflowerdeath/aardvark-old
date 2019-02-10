#include <experimental/filesystem>
#include <fstream>
#include "../js/bindings_host.hpp"

namespace fs = std::experimental::filesystem;

std::string get_selfpath() {
    char buffer[PATH_MAX];
    auto len = ::readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    buffer[len] = '\0';
    auto path = std::string(buffer);
    return path.substr(0, path.rfind('/'));
}

std::string read_file(fs::path path) {
    auto size = fs::file_size(path);
    auto result = std::string(size, ' ');
    auto stream = std::ifstream(path.string());
    stream.read(result.data(), size);
    return result;
}

int main() {
    auto host = aardvark::js::BindingsHost();
	    
    std::cout << get_selfpath() << std::endl;
    auto src_path = fs::path(get_selfpath()).append("../src/examples/src.js");
    auto src_str = read_file(src_path);
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
