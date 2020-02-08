#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <optional>
#include <aardvark_jsi/jsi.hpp>
#include "utils/event_loop.hpp"

namespace aardvark::js {

struct JsErrorLocation {
    std::string source_url = "";
    int line = -1;
    int column = -1;
};

using JsErrorHandler = std::function<void(jsi::Error)>;

class ModuleLoader {
  public:
    ModuleLoader(
        EventLoop* event_loop,
        jsi::Context& ctx,
        bool enable_source_maps,
        JsErrorHandler exception_handler);

    jsi::Value load_from_source(
        const std::string& source,
        const std::string& source_url = "",
        const std::string& source_map = "");

    jsi::Value load_from_file(const std::string& filepath);

    // void load_from_url(const std::string& url,
                       // std::function<void(JSValueRef)> callback);

    void handle_exception(jsi::Value exception);

    std::function<void(jsi::Error)> exception_handler;

  private:
    EventLoop* event_loop;
    jsi::Context* ctx;
    bool enable_source_maps;
    std::unordered_map<std::string, Value> source_maps;
    Value js_get_original_location;
    std::optional<JsErrorLocation> get_original_location(
        const JsErrorLocation& location);
};

}  // namespace aardvark::js
