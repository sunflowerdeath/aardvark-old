#pragma once

#include <optional>
#include <typeindex>
#include <memory>
#include <map>
#include <unordered_map>
#include <string>

#include "JavaScriptCore/JavaScript.h"
#include <nod/nod.hpp>

#include "../document.hpp"
#include "../element.hpp"
#include "../platforms/desktop/desktop_app.hpp"
#include "../platforms/desktop/desktop_window.hpp"
#include "../utils/event_loop.hpp"
#include "../utils/websocket.hpp"
#include "helpers.hpp"
#include "typedefs.hpp"
#include "module_loader.hpp"
#include "objects_index.hpp"

namespace aardvark::js {

class Typedefs;

class AnimationFrame {
  public:
    int add_callback(std::function<void()> callback) {
        id++;
        callbacks[id] = callback;
        return id;
    }

    void remove_callback(int id) {
        callbacks.erase(id);
    }

    void call_callbacks() {
        auto copy = callbacks;
        callbacks.clear();
        for (auto it : copy) it.second();
    }

  private:
    int id = 0;
    std::map<int, std::function<void()>> callbacks;
};


constexpr auto PROP_ATTR_STATIC =
    kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete;

typedef JSClassRef (*JSCreateClassCallback)(JSClassRef base_class);

class BindingsHost {
  public:
    BindingsHost();
    ~BindingsHost();
    void run();
    void stop();

    std::shared_ptr<JSGlobalContextWrapper> ctx;
    // std::function<void(JSValueRef)> exception_handler;
    std::shared_ptr<EventLoop> event_loop = std::make_shared<EventLoop>();
    std::unique_ptr<Typedefs> typedefs;
    std::unique_ptr<ModuleLoader> module_loader;
    std::optional<AnimationFrame> animation_frame;
    std::shared_ptr<DesktopApp> app;
    JSClassRef element_class;
    JSObjectRef element_constructor;
    // `type_info` can't be used as map key, it should be wrapped into
    // `type_index`
    std::unordered_map<std::type_index, JSClassRef> elements_classes;

    // Use optional to defer initialization
    std::optional<ObjectsIndex<DesktopApp>> desktop_app_window_list_index;
    std::optional<ObjectsIndex<DesktopWindow>> desktop_window_index;
    std::optional<ObjectsIndex<Document>> document_index;
    std::optional<ObjectsIndex<Element>> element_index;
    std::optional<ObjectsIndex<nod::connection>> signal_connection_index;
    std::optional<ObjectsIndex<Connection>> connection_index;
    std::optional<ObjectsIndex<Websocket>> websocket_index;

    static BindingsHost* get(JSContextRef ctx);

  private:
    bool is_running = false;

    void add_function(const char* name, JSObjectCallAsFunctionCallback function,
                      JSPropertyAttributes attributes = PROP_ATTR_STATIC);

    void add_object(const char* name, JSObjectRef object,
                    JSPropertyAttributes attributes = PROP_ATTR_STATIC);

    JSObjectRef add_constructor(
        const char* name, JSClassRef jsclass,
        JSObjectCallAsConstructorCallback call_as_constructor);

    JSClassRef get_element_js_class(Element* elem);

    void add_elem_class(
        const char* name, const std::type_info& elem_type,
        JSCreateClassCallback create_class,
        JSObjectCallAsConstructorCallback call_as_constructor);
};

}  // namespace aardvark::js
