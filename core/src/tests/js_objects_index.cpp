#include "Catch2/catch.hpp"
#include "../js/objects_index.hpp"

using namespace aardvark;

TEST_CASE("ObjectIndex", "[objects_index]" ) {
    class TestClass {
      public:
        TestClass(int value) : value(value){};
        int value;
    };

    auto definition = kJSClassDefinitionEmpty;
    definition.className = "TestClass";
    auto jsclass = JSClassCreate(&definition);
    auto ctx = JSGlobalContextCreate(nullptr);

    auto s_ptr = std::make_shared<TestClass>(1);

    SECTION("get_or_create_object") {
        auto index = js::ObjectsIndex<TestClass>(ctx, jsclass);
        auto object1 = index.get_or_create_object(s_ptr);
        auto object2 = index.get_or_create_object(s_ptr);
        REQUIRE(object1 == object2);
    }

    SECTION("get_object") {
        auto index = js::ObjectsIndex<TestClass>(ctx, jsclass);
        auto object1 = index.get_or_create_object(s_ptr);
        auto object2 = index.get_object(s_ptr.get());
        REQUIRE(object1 == object2);
    }

    SECTION("get_instance") {
        auto index = js::ObjectsIndex<TestClass>(ctx, jsclass);
        auto object1 = index.get_or_create_object(s_ptr);
        auto s_ptr2 = index.get_instance(object1);
        REQUIRE(s_ptr == s_ptr2);
    }

    SECTION("remove") {
        auto index = js::ObjectsIndex<TestClass>(ctx, jsclass);
        auto object1 = index.get_or_create_object(s_ptr);
        js::ObjectsIndex<TestClass>::remove(object1);
        auto object2 = index.get_or_create_object(s_ptr);
        REQUIRE(object1 != object2);
    }
}
