#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <variant>
#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

template <class T>
class ObjectsIndex {
    using JSClassGetter = std::function<JSClassRef(T*)>;

  public:
    ObjectsIndex(JSContextRef ctx,
                 std::variant<JSClassRef, JSClassGetter> js_class)
        : ctx(ctx), js_class(js_class){};

    // Returns native object corresponding to JS object.
    std::shared_ptr<T> get_native_object(JSObjectRef object) {
        auto record = static_cast<Record*>(JSObjectGetPrivate(object));
        return record->native_object;
    };

    // Creates JS object corresponding to the native object and stores it in the 
    // index. Uses `js_class` to determine class of the JS object.
    // Returns created JS object.
    JSObjectRef create_js_object(const std::shared_ptr<T>& s_ptr) {
        auto ptr = s_ptr.get();
        auto a_js_class = std::holds_alternative<JSClassRef>(js_class)
                              ? std::get<JSClassRef>(js_class)
                              : std::get<JSClassGetter>(js_class)(s_ptr.get());
        auto js_object = JSObjectMake(ctx, a_js_class, nullptr);
        index[ptr] = Record{s_ptr, js_object, &index};
        JSObjectSetPrivate(js_object, static_cast<void*>(&index[ptr]));
        return js_object;
    };

    // Returns JS object corresponding to the native object, creating it, if it
    // does not exists yet.
    JSObjectRef get_or_create_js_object(const std::shared_ptr<T>& s_ptr) {
        auto ptr = s_ptr.get();
        auto search = index.find(ptr);
        if (search != index.end()) {
            return search->second.js_object;
        } else {
            return create_js_object(s_ptr);
        }
    };

    // Returns JS object corresponding to the native object
    JSObjectRef get_js_object(T* ptr) { return index[ptr].js_object; };

    // Call this method when js object is destroyed to remove corresponding
    // record from the index
    static void remove(JSObjectRef object) {
        auto record = static_cast<Record*>(JSObjectGetPrivate(object));
        record->index->erase(record->native_object.get());
    };

  private:
    // This record is stored in the index, and pointer to it is stored in the
    // private data of the js object
    struct Record {
        std::shared_ptr<T> native_object;
        JSObjectRef js_object;
        std::unordered_map<T*, Record>* index;
    };
    JSContextRef ctx;
    std::variant<JSClassRef, JSClassGetter> js_class;
    std::unordered_map<T*, Record> index;
};

}  // namespace aardvark::js
