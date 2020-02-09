#include "module_loader.hpp"

#include <aardvark/utils/files_utils.hpp>
#include <aardvark/utils/log.hpp>
#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/mappers.hpp>
#include <experimental/filesystem>
#include <regex>

namespace fs = std::experimental::filesystem;

namespace aardvark::js {

// auto js_error_location_mapper =
    // new ObjectMapper<JsErrorLocation, std::string, int, int>(
        // {"sourceURL", &JsErrorLocation::source_url, str_mapper},
        // {"line", &JsErrorLocation::line, int_mapper},
        // {"column", &JsErrorLocation::column, int_mapper});

std::string get_source_map_url(const std::string& source) {
    static auto re = std::regex("\n//# sourceMappingURL=(.+)$");
    std::smatch match;
    if (std::regex_search(source, match, re)) return match[1];
    return "";
}

ModuleLoader::ModuleLoader(
    EventLoop* event_loop,
    jsi::Context* ctx,
    bool enable_source_maps,
    ErrorHandler error_handler)
    : event_loop(event_loop),
      ctx(ctx),
      enable_source_maps(enable_source_maps),
      error_handler(std::move(error_handler)) {
    /*
    if (enable_source_maps) {
        auto source_path =
            fs::path(utils::get_self_path()).append("getOriginalLocation.js");
        auto source = utils::read_text_file(source_path);
        auto this_object = ctx->object_make(nullptr);
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
    */
};

jsi::Value ModuleLoader::load_from_source(
    const std::string& source,
    const std::string& source_url,
    const std::string& source_map) {
    if (enable_source_maps && !source_url.empty() && !source_map.empty()) {
        source_maps.try_emplace(
            source_url,
            ctx->value_make_string(ctx->string_make_from_utf8(source_map)));
    }
    auto res = ctx->eval(source, nullptr, source_url);
    if (res.has_value()) return res.value();
    handle_error(res.error());
    return ctx->value_make_undefined();
}

jsi::Value ModuleLoader::load_from_file(const std::string& filepath) {
    // TODO check relative/absolute path
    auto full_filepath = fs::current_path().append(filepath);
    Log::info("[ModuleLoader] Load module from file {}", filepath);
    auto source = utils::read_text_file(full_filepath);
    auto source_map = std::string();
    /*
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
    */
    return ModuleLoader::load_from_source(source, full_filepath, source_map);
}

void ModuleLoader::handle_error(jsi::Error& error) {
    if (!error_handler) return;

    // auto location = js_error_location_mapper->from_js(ctx, exception);
    // auto error = JsError{
        // exception,                                  // value
        // aardvark::js::str_from_js(ctx, exception),  // text
        // location,                                   // location
        // get_original_location(location)             // original_location
    // };
    error_handler(error);
}

std::optional<jsi::ErrorLocation> ModuleLoader::get_original_location(
    const jsi::ErrorLocation& location) {
    return std::nullopt;

    /*
    if (!enable_source_maps || location.source_url.empty()) return std::nullopt;
    auto it = source_maps.find(location.source_url);
    if (it == source_maps.end()) return std::nullopt;
    auto source_map = it->second.get();

    auto func = JSValueToObject(ctx, js_get_original_location.get(), nullptr);
    JSValueRef args[] = {js_error_location_mapper->to_js(ctx, location),
                         source_map};
    JSValueRef exception;
    auto result = JSObjectCallAsFunction(ctx,        // ctx
                                         func,       // object
                                         nullptr,    // thisObject
                                         2,          // argumentCount
                                         args,       // arguments[],
                                         &exception  // exception
    );
    if (exception != nullptr) {
        Log::warn("Could not get original location from source map");
        return std::nullopt;
    }
    return js_error_location_mapper->from_js(ctx, result);
    */
}

}  // namespace aardvark::js
