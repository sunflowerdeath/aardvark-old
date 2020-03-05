#pragma once

#include <aardvark_jsi/jsi.hpp>
#include <string>

std::string test_function(
    aardvark::jsi::Context& ctx, int int_arg, bool bool_arg);
