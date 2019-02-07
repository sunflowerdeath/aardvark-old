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

    // Call this method when js object is destroyed to remove corresponding
    // record from the index
    void remove(JSObjectRef object) {
        auto ptr = static_cast<T*>(JSObjectGetPrivate(object));
        index.erase(ptr);
    };

    // Returns native object corresponding to JS object.
    std::shared_ptr<T> get_instance(JSObjectRef object) {
        auto ptr = static_cast<T*>(JSObjectGetPrivate(object));
        return index[ptr].instance;
    };

    // Creates JS object wrapper for native object and store in in the index
    // Returns created JS object.
    JSObjectRef create_record(const std::shared_ptr<T>& s_ptr) {
        auto ptr = s_ptr.get();
        auto object = JSObjectMake(ctx, jsclass, static_cast<void*>(ptr));
        index[ptr] = Record{s_ptr, object};
        return object;
    };

    // Returns JS object corresponding to the native object, creating it, if it
    // does not exists yet.
    JSObjectRef get_or_create_object(const std::shared_ptr<T>& s_ptr) {
        auto ptr = s_ptr.get();
        auto search = index.find(ptr);
        if (search != index.end()) {
            return search->second.object;
        } else {
            return create_record(s_ptr);
        }
    };

    // Returns JS object corresponding to the native object
    JSObjectRef get_object(T* ptr) {
        return index[ptr].object;
    };

  private:
    struct Record {
        std::shared_ptr<T> instance;
        JSObjectRef object;
    };
    JSContextRef ctx;
    JSClassRef jsclass;
    std::unordered_map<T*, Record> index;
};

}  // namespace aardvark::js
