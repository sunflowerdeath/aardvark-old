#include <initializer_list>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <jni.h>
#include <errno.h>
//#include <cassert>

#include <string> 

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include "../../../../core/src/layer.hpp"
#include "../../../../core/src/platforms/android/android_window.hpp"
#include "../../../../core/src/document.hpp"
#include "../../../../core/src/elements/elements.hpp"
#include "../../../../core/src/inline_layout/text_span.hpp"

#include "GrBackendSurface.h"
#include "GrContext.h"
#include "SkCanvas.h"
#include "SkSurface.h"
#include "gl/GrGLInterface.h"

#define GR_GL_FRAMEBUFFER_BINDING 0x8CA6
#define GR_GL_BGRA8 0x93A1
#define GR_GL_RGBA8 0x8058

#define LOG_INFO(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOG_WARN(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

struct AppState {
    AppState() {
        eglBindAPI(EGL_OPENGL_ES_API);

    }
    
    bool is_active;
    float color;
    float pos = 0;
    // sk_sp<GrContext> gr_context;
    std::shared_ptr<aardvark::AndroidWindow> window;
    // std::shared_ptr<aardvark::Layer> layer;
    std::shared_ptr<aardvark::Document> document;
    std::shared_ptr<aardvark::elements::Align> align;
    
    void init(ANativeWindow* native_window) {
        window = std::make_shared<aardvark::AndroidWindow>(native_window);
        // gr_context = GrContext::MakeGL();
        // layer = aardvark::Layer::make_screen_layer(gr_context);
        document = std::make_shared<aardvark::Document>();
        align = std::make_shared<aardvark::elements::Align>(
            std::make_shared<aardvark::elements::FixedSize>(
                std::make_shared<aardvark::elements::Background>(SK_ColorRED),
                aardvark::Size{200, 200}),
            aardvark::elements::EdgeInsets{
                aardvark::Value::abs(200), // left
                aardvark::Value::abs(200)  // top
            });

        auto text =
            UnicodeString((UChar*)u"Lorem ipsum dolor sit amet, consectetur");
        SkPaint paint;
        paint.setColor(SK_ColorWHITE);
        paint.setTextSize(24);
        paint.setAntiAlias(true);
        auto span =
            std::make_shared<aardvark::inline_layout::TextSpan>(text, paint);
        auto paragraph = std::make_shared<aardvark::elements::Paragraph>(
            std::vector<std::shared_ptr<aardvark::inline_layout::Span>>{span},
            aardvark::inline_layout::LineMetrics::from_paint(paint).scale(1.5));
        auto align_paragraph = std::make_shared<aardvark::elements::Align>(
            std::make_shared<aardvark::elements::FixedSize>(
                paragraph, aardvark::Size{200, 200}),
            aardvark::elements::EdgeInsets{
                aardvark::Value::abs(200), // left
                aardvark::Value::abs(500)  // top
            });
        auto stack = std::make_shared<aardvark::elements::Stack>(
            std::vector<std::shared_ptr<aardvark::Element>>{
                align, align_paragraph});
        document->set_root(stack);
    }
    
    void term() {
        window.reset();
    }
};

void draw_frame(AppState *app_state) {
    app_state->align->insets.left = aardvark::Value::abs(200 + app_state->pos);
    app_state->align->change();
    app_state->document->paint();
    app_state->window->swap();
    // glFlush();
    /*
	glClearColor(0, app_state->color, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	SkCanvas* canvas = app_state->layer->canvas;
	SkPaint paint;
	const char* message = "Hello world";
	paint.setColor(SK_ColorWHITE);
	paint.setTextSize(50);
    canvas->drawText(
		message,
		strlen(message),
		SkIntToScalar(100),
        SkIntToScalar(100),
		paint
	);
	canvas->flush();
    */
}

void handle_cmd(android_app *app, int32_t cmd) {
  AppState *app_state = (AppState *)app->userData;
  switch (cmd) {
  case APP_CMD_INIT_WINDOW:
    LOG_INFO("APP_CMD_INIT_WINDOW");
    app_state->init(app->window);
    app_state->is_active = true;
    break;
  case APP_CMD_TERM_WINDOW:
    LOG_INFO("APP_CMD_TERM_WINDOW");
    // The window is being hidden or closed, clean it up.
    app_state->term();
    app_state->is_active = false;
    break;
  case APP_CMD_GAINED_FOCUS:
    LOG_INFO("APP_CMD_GAINED_FOCUS");
    app_state->is_active = true;
    break;
  case APP_CMD_LOST_FOCUS:
    LOG_INFO("APP_CMD_LOST_FOCUS");
    app_state->is_active = false;
    break;
  }
}

/**
 * Main entry point of a native application that is using
 * android_native_app_glue. It runs in its own thread, with its own event loop
 * for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
    auto app_state = AppState();
    app->userData = &app_state;
    app->onAppCmd = handle_cmd;

    while (true) {
        android_poll_source* source;
        // If not animating, block forever waiting for events.
        // If animating, loop until all events are read, then continue
        // to draw the next frame of animation.
        while (
        	// Value >= 0 when this function requires caller to handle events
        	ALooper_pollAll(
        		app_state.is_active ? 0 : -1, // timeout
        		NULL, NULL, (void**)&source
        	) >= 0
        ) {
            // Process event
            if (source != NULL) {
            	// This will call app->onAppCmd
            	source->process(app, source);
            }
            /*
            if (state->destroyRequested != 0) {
                term_display(&app);
                return;
            }
            */
        }

    	if (app_state.is_active) {
            app_state.pos += 1;
            if (app_state.pos > 100) app_state.pos = 0;
            // app_state.color += .01f;
            // if (app_state.color > 1) app_state.color = 0;
    		draw_frame(&app_state);
    	}
    }
}
