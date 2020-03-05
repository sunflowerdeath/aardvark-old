#include "animation_frame.hpp"

#include "host.hpp"

namespace aardvark::js {

int request_animation_frame(jsi::Context& ctx, std::function<void()> callback) {
    auto host = static_cast<Host*>(ctx.user_pointer);
    return host->animation_frame.add_callback(std::move(callback));
}

void cancel_animation_frame(jsi::Context& ctx, int id) {
    auto host = static_cast<Host*>(ctx.user_pointer);
    host->animation_frame.remove_callback(id);
}

}  // namespace aardvark::js
