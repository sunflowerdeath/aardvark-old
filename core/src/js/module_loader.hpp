#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "JavaScriptCore/JavaScript.h"
#include "../utils/event_loop.hpp"

namespace aardvark::js {

struct JsErrorLocation {
    std::string source_url = "";
    int line = -1;
    int column = -1;
};

struct JsError {
    JSValueRef value;
    std::string text;
    // TODO when no location
    JsErrorLocation location;
    JsErrorLocation original_location;
};

class ModuleLoader {
  public:
    ModuleLoader(EventLoop* event_loop, JSGlobalContextRef ctx,
                 bool enable_source_maps)
        : event_loop(event_loop),
          ctx(ctx),
          enable_source_maps(enable_source_maps){};

    JSValueRef load_from_source(const std::string& source,
                                const std::string& source_url = "",
                                const std::string& source_map = "");

    JSValueRef load_from_file(const std::string& filepath);

    // void load_from_url(const std::string& url,
                       // std::function<void(JSValueRef)> callback);

    void handle_exception(JSValueRef exception);

    std::function<void(JsError)> exception_handler;

  private:
    EventLoop* event_loop;
    JSGlobalContextRef ctx;
    bool enable_source_maps;
    std::unordered_map<std::string, std::string> source_maps;
    // JSErrorLocation get_original_location(JSErrorLocation location);
};

}  // namespace aardvark::js
