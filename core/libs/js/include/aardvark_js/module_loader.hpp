#pragma once

#include <aardvark/utils/event_loop.hpp>
#include <aardvark_jsi/jsi.hpp>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "../generated/error_location_api.hpp"

namespace aardvark::js {

using ErrorHandler =
    std::function<void(jsi::Error&, std::optional<jsi::ErrorLocation>)>;

class ModuleLoader {
  public:
    ModuleLoader(
        EventLoop* event_loop,
        jsi::Context* ctx,
        bool enable_source_maps,
        ErrorHandler error_handler);

    jsi::Value load_from_source(
        const std::string& source,
        const std::string& source_url = "",
        const std::string& source_map = "");

    // jsi::Value load_from_file(const std::string& filepath);

    // void load_from_url(const std::string& url,
                       // std::function<void(JSValueRef)> callback);

    void handle_error(jsi::Error& error);

  private:
    ErrorHandler error_handler;
    EventLoop* event_loop;
    jsi::Context* ctx;
    bool enable_source_maps;
    aardvark_js_api::ErrorLocationApi api;
    std::unordered_map<std::string, jsi::Value> source_maps;
    std::optional<jsi::Object> js_get_original_location;
    std::optional<jsi::ErrorLocation> get_original_location(
        const jsi::ErrorLocation& location);
};

}  // namespace aardvark::js
