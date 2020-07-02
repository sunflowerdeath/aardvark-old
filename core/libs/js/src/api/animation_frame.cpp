#include "api/animation_frame.hpp"

#if ADV_PLATFORM_DESKTOP == 1
    #include "host.hpp"
    #define HOST_TYPE Host
#elif defined(ADV_PLATFORM_ANDROID)
    #include "android_host.hpp"
    #define HOST_TYPE AndroidHost
#endif

namespace aardvark::js {

int request_animation_frame(jsi::Context& ctx, std::function<void()> callback) {
    auto host = static_cast<HOST_TYPE*>(ctx.user_pointer);
    return host->animation_frame.add_callback(std::move(callback));
}

void cancel_animation_frame(jsi::Context& ctx, int id) {
    auto host = static_cast<HOST_TYPE*>(ctx.user_pointer);
    host->animation_frame.remove_callback(id);
}

}  // namespace aardvark::js
