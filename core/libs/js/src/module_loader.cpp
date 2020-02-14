#include "module_loader.hpp"

#include <aardvark/utils/files_utils.hpp>
#include <aardvark/utils/log.hpp>
#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/mappers.hpp>
#include <experimental/filesystem>
#include <regex>

namespace fs = std::experimental::filesystem;

namespace aardvark::js {

std::string get_source_map_url(const std::string& source) {
    static auto re = std::regex("\n//# sourceMappingURL=(.+)$");
    std::smatch match;
    if (std::regex_search(source, match, re)) return match[1];
    return "";
}

auto get_original_location_src = std::string(
#include "../generated/getOriginalLocation.js"
);

ModuleLoader::ModuleLoader(
    EventLoop* event_loop,
    jsi::Context* ctx,
    bool enable_source_maps,
    ErrorHandler error_handler)
    : event_loop(event_loop),
      ctx(ctx),
      enable_source_maps(enable_source_maps),
      error_handler(std::move(error_handler)),
      api(ctx) {
    if (enable_source_maps) {
        ctx->eval(get_original_location_src, nullptr, "getOriginalLocation.js");
        js_get_original_location = ctx->get_global_object()
                                       .get_property("getOriginalLocation")
                                       .value()
                                       .to_object()
                                       .value();
    }
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
    return ctx->value_make_null();
}

jsi::Value ModuleLoader::load_from_file(const std::string& filepath) {
    // TODO check relative/absolute path
    auto full_filepath = fs::current_path().append(filepath);
    Log::info("[ModuleLoader] Load module from file {}", filepath);
    auto source = utils::read_text_file(full_filepath);
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

void ModuleLoader::handle_error(jsi::Error& error) {
    auto loc = error.location();
    auto orig_loc =
        loc.has_value() ? get_original_location(loc.value()) : std::nullopt;
    error_handler(error, orig_loc);
}

std::optional<jsi::ErrorLocation> ModuleLoader::get_original_location(
    const jsi::ErrorLocation& location) {
    if (!enable_source_maps || location.source_url.empty()) return std::nullopt;
    auto it = source_maps.find(location.source_url);
    if (it == source_maps.end()) return std::nullopt;
    auto source_map = it->second;

    auto js_loc = api.ErrorLocation_mapper->to_js(*ctx, location);
    auto args = std::vector<jsi::Value>{js_loc, source_map};
    auto res = js_get_original_location->call_as_function(nullptr, args);
    if (!res.has_value()) {
        Log::warn("Could not get original location from source map");
        return std::nullopt;
    }
    return api.ErrorLocation_mapper->from_js(*ctx, res.value());
}

}  // namespace aardvark::js
