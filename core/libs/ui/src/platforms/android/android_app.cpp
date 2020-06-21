#include "platforms/android/android_app.hpp"

#include "platforms/android/android_binary_channel.hpp"

namespace aardvark {

AndroidApp::AndroidApp(jobject activity, int width, int height)
    : activity(activity), width(width), height(height) {
    auto gr_context = GrContext::MakeGL();
    auto screen = Layer::make_screen_layer(gr_context);
    document = std::make_shared<Document>(gr_context, screen);
}

void AndroidApp::update() { document->render(); }

// void AndroidApp::handle_system_message(json message) { document->handle_event
// }

}  // namespace aardvark
