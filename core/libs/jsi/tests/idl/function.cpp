#include "function.hpp"

std::string test_function(
    aardvark::jsi::Context& ctx, int int_arg, bool bool_arg) {
    return std::to_string(int_arg + bool_arg);
}
