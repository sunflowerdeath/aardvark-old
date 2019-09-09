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

enum class PointerTool { touch, mouse, other };

enum class PointerAction {
    pointer_up,
    pointer_down,
    pointer_move,
    button_press,
    button_release,
    scroll
};

struct PointerEvent {
    PointerTool tool;
    int pointer_id;
    PointerAction action;
    float left;
    float top;
};

struct ButtonEvent : public PointerEvent {
    int button_id;
};

struct MouseEnterEvent {};

struct MouseLeaveEvent {};

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
                 MouseEnterEvent, MouseLeaveEvent, PointerEvent, KeyEvent>;

}  // namespace aardvark
