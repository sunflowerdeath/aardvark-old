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
    int left;
    int top;
};

struct WindowCloseEvent {};

struct WindowMinimizeEvent {};

struct WindowRestoreEvent {};

struct WindowResizeEvent {
    int width;
    int height;
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

struct CharEvent {
    int codepoint;
};

// Scroll

struct ScrollEvent {
    float left;
    float top;
};

using Event =
    std::variant<WindowEvent, PointerEvent, KeyEvent, CharEvent, ScrollEvent>;

using EventHandler = std::function<void(Event)>;

}  // namespace aardvark
