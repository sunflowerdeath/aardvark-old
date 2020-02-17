#pragma once

#include <variant>

struct UnionTypeA { int prop; };
struct UnionTypeB {};

using TestUnion = std::variant<UnionTypeA, UnionTypeB>;
