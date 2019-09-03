#include "module_loader.hpp"

#include <experimental/filesystem>
#include "../utils/files_utils.hpp"
#include "helpers.hpp"

namespace aardvark::js {

namespace fs = std::experimental::filesystem;

JSValueRef ModuleLoader::load_from_source(const std::string& source,
                                          const std::string& source_url,
                                          const std::string& source_map) {
    if (enable_source_maps && !source_url.empty() && !source_map.empty()) {
        source_maps[source_url] = source_map;
    }
    auto js_src = JSStringCreateWithUTF8CString(source.c_str());
    auto js_source_url =
        source_url.empty() ? nullptr
                           : JSStringCreateWithUTF8CString(source_url.c_str());
    auto exception = JSValueRef();
    auto result = JSEvaluateScript(ctx,            // ctx,
                                   js_src,         // script
                                   nullptr,        // thisObject,
                                   js_source_url,  // sourceURL,
                                   1,              // startingLineNumber,
                                   &exception      // exception
    );
    JSStringRelease(js_src);
    if (js_source_url != nullptr) JSStringRelease(js_source_url);
    if (exception != nullptr) handle_exception(exception);
    return result;
};

JSValueRef ModuleLoader::load_from_file(const std::string& filepath) {
    // TODO check relative/absolute path
    auto full_filepath = fs::current_path().append(filepath);
    // TODO check if exists
    auto source = aardvark::utils::read_text_file(full_filepath);
    // TODO extract source map
    return ModuleLoader::load_from_source(source, full_filepath);
}

JsErrorLocation location_from_js(JSContextRef ctx, JSValueRef value) {
    auto object = JSValueToObject(ctx, value, nullptr);
    auto loc = JsErrorLocation();
    map_prop_from_js<std::string, str_from_js>(ctx, object, "sourceURL",
                                               &loc.source_url);
    map_prop_from_js<int, int_from_js>(ctx, object, "line", &loc.line);
    map_prop_from_js<int, int_from_js>(ctx, object, "column", &loc.column);
    return loc;
}

void ModuleLoader::handle_exception(JSValueRef exception) {
    if (!exception_handler) return;
    auto error = JsError{
        exception,                                  // value
        aardvark::js::str_from_js(ctx, exception),  // text
        location_from_js(ctx, exception)            // location
    };
    // TODO get original location
    exception_handler(error);
}

}  // namespace aardvark::js
