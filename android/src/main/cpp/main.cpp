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
    bool is_active;
    float color;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    sk_sp<SkSurface> skia_surface;
};

void init_skia(AppState* app_state) {
    const int kStencilBits = 8; // Skia needs 8 stencil bits
    const int kMsaaSampleCount = 0; //4

    // These values may be different on some devices
    const SkColorType colorType = kRGBA_8888_SkColorType;
    const GrGLenum colorFormat = GR_GL_RGBA8;

    EGLint width;
    EGLint height;
    eglQuerySurface(app_state->display, app_state->surface, EGL_WIDTH, &width);
    eglQuerySurface(app_state->display, app_state->surface, EGL_HEIGHT, &height);

    // Setup GrContext
	sk_sp<GrContext> grContext(GrContext::MakeGL());

	// Wrap the frame buffer object attached to the screen in a Skia render target
	
	// Get currently bound framebuffer object id
	// This is redunant as buffer currently bound to screen should be always 0
    GrGLint buffer;
	glGetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer);
    GrGLFramebufferInfo info;
    info.fFBOID = (GrGLuint) buffer; // Framebuffer object id
	info.fFormat = colorFormat;
	GrBackendRenderTarget target(width, height, kMsaaSampleCount,
								 kStencilBits, info);

	// setup SkSurface
    // To use distance field text, use commented out SkSurfaceProps instead
    // SkSurfaceProps props(SkSurfaceProps::kUseDeviceIndependentFonts_Flag,
    //                      SkSurfaceProps::kLegacyFontHost_InitType);
    SkSurfaceProps props(SkSurfaceProps::kLegacyFontHost_InitType);
    app_state->skia_surface = SkSurface::MakeFromBackendRenderTarget(
		grContext.get(),
		target,
		kBottomLeft_GrSurfaceOrigin,
		colorType,
		nullptr,
		&props
	);
}

EGLConfig choose_egl_config(EGLDisplay display) {
	// Has at least 8 bits per color and supports creating window surfaces
    const EGLint attribs[] = {
    	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE // The list is terminated with EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs = 0;
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    // TODO assert numConfigs > 0
    return config;
}

void log_opengl_info() {
    auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
    for (auto name : opengl_info) {
        auto info = glGetString(name);
        LOG_INFO("OpenGL Info: %s", info);
    }
}

int init_display(AppState* app_state, android_app* app) {
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, 0, 0);

	eglBindAPI(EGL_OPENGL_ES_API);

    EGLConfig config = choose_egl_config(display);

	const EGLint EGLContextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    EGLContext context = eglCreateContext(display, config, NULL, EGLContextAttribs);
    
    EGLSurface surface = eglCreateWindowSurface(display, config, app->window, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOG_WARN("Unable to eglMakeCurrent");
        return -1;
    }

    app_state->display = display;
    app_state->context = context;
    app_state->surface = surface;

    init_skia(app_state);

    app_state->is_active = true;

    log_opengl_info();

    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    return 0;
}

void draw_frame(AppState *app_state) {
	glClearColor(0, app_state->color, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	SkCanvas* canvas = app_state->skia_surface->getCanvas();
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

	eglSwapBuffers(app_state->display, app_state->surface);
}

void term_display(AppState* app_state) {
    if (app_state->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(app_state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (app_state->context != EGL_NO_CONTEXT) {
		   eglDestroyContext(app_state->display, app_state->context);
		}
		if (app_state->surface != EGL_NO_SURFACE) {
		    eglDestroySurface(app_state->display, app_state->surface);
		}
		eglTerminate(app_state->display);
    }
    app_state->display = EGL_NO_DISPLAY;
    app_state->context = EGL_NO_CONTEXT;
    app_state->surface = EGL_NO_SURFACE;
    app_state->is_active = false;
}

void handle_cmd(android_app* app, int32_t cmd) {
	AppState* app_state = (AppState*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
        	LOG_INFO("APP_CMD_INIT_WINDOW");
            init_display(app_state, app);
            break;
        case APP_CMD_TERM_WINDOW:
        	LOG_INFO("APP_CMD_TERM_WINDOW");
            // The window is being hidden or closed, clean it up.
            term_display(app_state);
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
 * Main entry point of a native application that is using android_native_app_glue. 
 * It runs in its own thread, with its own event loop for receiving input events 
 * and doing other things.
 */
void android_main(struct android_app* app) {
    AppState app_state;
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
    	    app_state.color += .01f;
            if (app_state.color > 1) app_state.color = 0;
    		draw_frame(&app_state);
    	}
    }
}
