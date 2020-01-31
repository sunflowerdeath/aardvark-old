#include "function.hpp"

std::string test_function(int int_arg, bool bool_arg) {
    return std::to_string(int_arg + bool_arg);
}
