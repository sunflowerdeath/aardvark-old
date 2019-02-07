#include <iostream>
#include <memory>
#include <unordered_map>
#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

template <class T>
class ObjectsIndex {
  public:
    ObjectsIndex(JSContextRef ctx, JSClassRef jsclass)
        : ctx(ctx), jsclass(jsclass){};

    // Returns native object corresponding to JS object.
    std::shared_ptr<T> get_instance(JSObjectRef object) {
        auto record = static_cast<Record*>(JSObjectGetPrivate(object));
        return record->native_object;
    };

    // Creates JS object wrapper for native object and store in in the index
    // Returns created JS object.
    JSObjectRef create_record(const std::shared_ptr<T>& s_ptr) {
        auto ptr = s_ptr.get();
        auto js_object = JSObjectMake(ctx, jsclass, nullptr);
        index[ptr] = Record{s_ptr, js_object, &index};
        JSObjectSetPrivate(js_object, static_cast<void*>(&index[ptr]));
        return js_object;
    };

    // Returns JS object corresponding to the native object, creating it, if it
    // does not exists yet.
    JSObjectRef get_or_create_object(const std::shared_ptr<T>& s_ptr) {
        auto ptr = s_ptr.get();
        auto search = index.find(ptr);
        if (search != index.end()) {
            return search->second.js_object;
        } else {
            return create_record(s_ptr);
        }
    };

    // Returns JS object corresponding to the native object
    JSObjectRef get_object(T* ptr) {
        return index[ptr].js_object;
    };

    // Call this method when js object is destroyed to remove corresponding
    // record from the index
    static void remove(JSObjectRef object) {
        auto record = static_cast<Record*>(JSObjectGetPrivate(object));
        record->index->erase(record->native_object.get());
    };

  private:
    struct Record {
        std::shared_ptr<T> native_object;
        JSObjectRef js_object;
        std::unordered_map<T*, Record>* index;
    };
    JSContextRef ctx;
    JSClassRef jsclass;
    std::unordered_map<T*, Record> index;
};

}  // namespace aardvark::js
