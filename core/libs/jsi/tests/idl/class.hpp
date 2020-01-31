#pragma once

#include <string>

struct TestClass {
  public:
    TestClass(int int_prop, bool readonly_prop)
        : int_prop(int_prop), readonly_prop(readonly_prop) {};
    int int_prop;
    bool readonly_prop;
    std::string method_name(int argName) {
        return std::to_string(argName + int_prop);
    }
};
