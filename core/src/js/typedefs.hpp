#pragma once

#include "bindings_host.hpp"
#include "check_types.hpp"

namespace aardvark::js {

class BindingsHost;

class Typedefs {
  public:
    Typedefs(BindingsHost* host);
    check_types::Checker color_component;
    check_types::Checker color;
    check_types::Checker value;
    check_types::Checker alignment;
    check_types::Checker element;

  private:
    BindingsHost* host;
};

}  // namespace aardvark::js
