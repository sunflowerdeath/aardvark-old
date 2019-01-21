#pragma once

#include <variant>

namespace aardvark {

struct WindowFocusEvent {};
struct WindowBlurEvent {};
struct WindowMoveEvent {
  WindowMoveEvent(float left, float top) : left(left), top(top){};
  double left;
  double top;
};
struct WindowCloseEvent {};
struct WindowMinimizeEvent {};
struct WindowRestoreEvent {};

struct MouseEnterEvent {};
struct MouseLeaveEvent {};
struct MouseMoveEvent {
  MouseMoveEvent(float left, float top) : left(left), top(top){};
  double left;
  double top;
};
struct MouseDownEvent {
  int button;
  int mods;
};
struct MouseUpEvent {
  int button;
  int mods;
};

enum class KeyAction { Press, Release, Repeat };

struct KeyEvent {
  int key;
  int scancode;
  KeyAction action;
  int mods;
};

using Event =
    std::variant<WindowFocusEvent, WindowBlurEvent, WindowMoveEvent,
                 WindowCloseEvent, WindowMinimizeEvent, WindowRestoreEvent,
                 MouseEnterEvent, MouseLeaveEvent, MouseMoveEvent,
                 MouseDownEvent, MouseUpEvent, KeyEvent>;

};  // namespace aardvark
