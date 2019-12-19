# aardvark::jsi

`aardvark::jsi` is a JavaScript interface library.

- Provides abstraction level over different JS-engines (currently supports JavaScriptCore and quickjs).
- Automatic memory management.
- Includes collection of useful helpers for creating bindings.

## Overview

- `jsi.h` &ndash; Common interface definition
- `jsc.h` &ndash; Implementation for JavaScriptCore
- `qjs.h` &ndash; Implementation for quickjs
- `mappers.hpp` &ndash; Helpers for mapping types between JS and C++
