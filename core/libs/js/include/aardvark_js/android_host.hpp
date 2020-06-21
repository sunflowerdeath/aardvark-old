#pragma once

#include <aardvark/channels.hpp>
#include <aardvark/platforms/android/android_app.hpp>
#include <aardvark/platforms/android/android_binary_channel.hpp>
#include <aardvark_jsi/jsi.hpp>
#include <aardvark_jsi/qjs.hpp>
// #include <aardvark/utils/jni_event_loop.hpp>

#include "../generated/android_api.hpp"
// #include "api/animation_frame.hpp"

#include "module_loader.hpp"

namespace aardvark::js {

class AndroidHost {
  public:
    AndroidHost(
        jobject system_channel, jobject activity, int width, int height);

    void update() {
        app->update();
    }
    
    std::optional<aardvark_js_api::AndroidApi> api;
    std::shared_ptr<jsi::Context> ctx;
    std::optional<ModuleLoader> module_loader;
    ChannelManager channel_manager = ChannelManager();
    std::shared_ptr<AndroidApp> app;
};

}  // namespace aardvark::js
