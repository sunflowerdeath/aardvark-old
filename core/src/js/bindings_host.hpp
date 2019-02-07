#pragma once

#include <optional>
#include "JavaScriptCore/JavaScript.h"
#include "objects_index.hpp"
#include "../platforms/desktop/desktop_app.hpp"
#include "../platforms/desktop/desktop_window.hpp"

namespace aardvark::js {

const auto property_attributes_immutable =
    kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete;

class BindingsHost {
  public:
    BindingsHost();
    ~BindingsHost();
    // Use optional to defer initialization
    std::optional<ObjectsIndex<DesktopApp>> desktop_app_index;
    std::optional<ObjectsIndex<DesktopWindow>> desktop_window_index;
    JSGlobalContextRef ctx;
  private:
    void add_function(
        const char* name, JSObjectCallAsFunctionCallback function,
        JSPropertyAttributes attributes = property_attributes_immutable);
    void add_object(
        const char* name, JSObjectRef object,
        JSPropertyAttributes attributes = property_attributes_immutable);
    void add_constructor(const char* name, JSClassRef jsclass,
                         JSObjectCallAsConstructorCallback call_as_constructor);
};

BindingsHost* get_bindings_host(JSContextRef ctx);

std::string jsstring_to_std(JSStringRef jsstring);

}  // namespace aardvark::js
