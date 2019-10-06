#pragma once

#include <variant>
#include <functional>

namespace aardvark {

// Window

struct WindowFocusEvent {};

struct WindowBlurEvent {};

struct WindowCursorEnterEvent {};

struct WindowCursorLeaveEvent {};

struct WindowMoveEvent {
    float left;
    float top;
};

struct WindowCloseEvent {};

struct WindowMinimizeEvent {};

struct WindowRestoreEvent {};

struct WindowResizeEvent {
    float width;
    float height;
};

using WindowEvent =
    std::variant<WindowFocusEvent, WindowBlurEvent, WindowCursorEnterEvent,
                 WindowCursorLeaveEvent, WindowMoveEvent, WindowCloseEvent,
                 WindowMinimizeEvent, WindowRestoreEvent, WindowResizeEvent>;

// Pointer event

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
    int timestamp;
    PointerTool tool;
    int pointer_id;
    PointerAction action;
    float left;
    float top;
    int button;
};

// Keyboard

enum class KeyAction { Press, Release, Repeat };

struct KeyEvent {
    int key;
    int scancode;
    KeyAction action;
    int mods;
};

// Scroll

struct ScrollEvent {
    float top;
    float left;
};

using Event = std::variant<WindowEvent, PointerEvent, KeyEvent, ScrollEvent>;

using EventHandler = std::function<void(Event)>;

}  // namespace aardvark
