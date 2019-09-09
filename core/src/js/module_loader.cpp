#include "module_loader.hpp"

#include <experimental/filesystem>
#include <regex>
#include "../utils/log.hpp"
#include "../utils/files_utils.hpp"

namespace aardvark::js {

namespace fs = std::experimental::filesystem;

JsErrorLocation js_error_location_from_js(JSContextRef ctx, JSValueRef value) {
    auto object = JSValueToObject(ctx, value, nullptr);
    auto loc = JsErrorLocation();
    map_prop_from_js<std::string, str_from_js>(ctx, object, "sourceURL",
                                               &loc.source_url);
    map_prop_from_js<int, int_from_js>(ctx, object, "line", &loc.line);
    map_prop_from_js<int, int_from_js>(ctx, object, "column", &loc.column);
    return loc;
}

JSValueRef js_error_location_to_js(JSContextRef ctx,
                                   const JsErrorLocation& location) {
    auto object = JSObjectMake(ctx, nullptr, nullptr);
    map_prop_to_js<std::string, str_to_js>(ctx, object, "sourceURL",
                                           location.source_url);
    map_prop_to_js<int, int_to_js>(ctx, object, "line", location.line);
    map_prop_to_js<int, int_to_js>(ctx, object, "column", location.column);
    return object;
}

std::string get_source_map_url(const std::string& source) {
    static auto re = std::regex("\n//# sourceMappingURL=(.+)$");
    std::smatch match;
    if (std::regex_search(source, match, re)) return match[1];
    return "";
}

ModuleLoader::ModuleLoader(EventLoop* event_loop,
                           std::weak_ptr<JSGlobalContextWrapper> ctx_wptr,
                           bool enable_source_maps)
    : event_loop(event_loop),
      ctx_wptr(ctx_wptr),
      enable_source_maps(enable_source_maps) {
    if (enable_source_maps) {
        auto ctx = ctx_wptr.lock()->get();
        auto source_path =
            fs::path(utils::get_self_path()).append("getOriginalLocation.js");
        auto source = aardvark::utils::read_text_file(source_path);
        auto this_object = JSObjectMake(ctx, nullptr, nullptr);
        JSEvaluateScript(ctx,                            // ctx,
                         JsStringWrapper(source).get(),  // script
                         this_object,                    // thisObject,
                         nullptr,                        // sourceURL,
                         1,                              // startingLineNumber,
                         nullptr                         // exception
        );
        auto value = JSObjectGetProperty(
            ctx, this_object, JsStringWrapper("getOriginalLocation").get(),
            nullptr);
        js_get_original_location = JsValueWrapper(ctx_wptr, value);
    }
};

JSValueRef ModuleLoader::load_from_source(const std::string& source,
                                          const std::string& source_url,
                                          const std::string& source_map) {
    if (ctx_wptr.expired()) return nullptr;
    auto ctx = ctx_wptr.lock()->get();

    if (enable_source_maps && !source_url.empty() && !source_map.empty()) {
        source_maps.try_emplace(source_url,  // key
                                ctx_wptr, str_to_js(ctx, source_map));
    }
    auto js_src = JsStringWrapper(source);
    auto js_source_url =
        source_url.empty() ? nullptr
                           : JSStringCreateWithUTF8CString(source_url.c_str());
    auto exception = JSValueRef();
    auto result = JSEvaluateScript(ctx,            // ctx,
                                   js_src.get(),   // script
                                   nullptr,        // thisObject,
                                   js_source_url,  // sourceURL,
                                   1,              // startingLineNumber,
                                   &exception      // exception
    );
    if (js_source_url != nullptr) JSStringRelease(js_source_url);
    if (exception != nullptr) handle_exception(exception);
    return result;
}

JSValueRef ModuleLoader::load_from_file(const std::string& filepath) {
    // TODO check relative/absolute path
    auto full_filepath = fs::current_path().append(filepath);
    Log::info("[ModuleLoader] Load module from file {}", filepath);
    auto source = aardvark::utils::read_text_file(full_filepath);
    auto source_map = std::string();
    if (enable_source_maps) {
        auto source_map_url = get_source_map_url(source);
        if (!source_map_url.empty()) {
            auto source_map_path = fs::path(source_map_url);
            if (source_map_path.is_relative()) {
                source_map_path = full_filepath.parent_path() / source_map_path;
            }
            source_map = utils::read_text_file(source_map_path);
            Log::info("[ModuleLoader] Load external source map from {}",
                      source_map_path.u8string());
        }
    }
    return ModuleLoader::load_from_source(source, full_filepath, source_map);
}

void ModuleLoader::handle_exception(JSValueRef exception) {
    if (!exception_handler) return;

    if (ctx_wptr.expired()) return;
    auto ctx = ctx_wptr.lock()->get();

    auto location = js_error_location_from_js(ctx, exception);
    auto error = JsError{
        exception,                                  // value
        aardvark::js::str_from_js(ctx, exception),  // text
        location,                                   // location
        get_original_location(location)             // original_location
    };
    exception_handler(error);
}

std::optional<JsErrorLocation> ModuleLoader::get_original_location(
    const JsErrorLocation& location) {
    if (ctx_wptr.expired()) return std::nullopt;
    auto ctx = ctx_wptr.lock()->get();

    if (!enable_source_maps || location.source_url.empty()) return std::nullopt;
    auto it = source_maps.find(location.source_url);
    if (it == source_maps.end()) return std::nullopt;
    auto object = JSValueToObject(ctx, js_get_original_location.get(), nullptr);
    JSValueRef args[] = {js_error_location_to_js(ctx, location),
                         it->second.get()};
    auto exception = JSValueRef();
    auto result = JSObjectCallAsFunction(ctx,        // ctx
                                         object,     // object
                                         nullptr,    // thisObject
                                         2,          // argumentCount
                                         args,       // arguments[],
                                         &exception  // exception
    );
    if (exception != nullptr) {
        Log::warn("Could not get original location from source map");
        return std::nullopt;
    }
    return js_error_location_from_js(ctx, result);
}

}  // namespace aardvark::js