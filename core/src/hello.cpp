#include <iostream>
#include <string>

#include <SDL.h>
#include "fmt/format.h"

#include <GL/gl.h>

// skia
#include "GrBackendSurface.h"
#include "GrContext.h"
#include "SkCanvas.h"
#include "SkSurface.h"
#include "gl/GrGLInterface.h"

#include "JavaScriptCore/JavaScript.h"

// magic numbers from skia/src/gpu/gl/GrGLDefines.h
#define GR_GL_FRAMEBUFFER_BINDING 0x8CA6
#define GR_GL_BGRA8 0x93A1
#define GR_GL_RGBA8 0x8058

std::string greet(std::string name) { return fmt::format("Hello, {}!", name); }

void handle_sdl_error() {
  const char* error = SDL_GetError();
  std::cout << fmt::format("SDL Error: {}\n", error);
  SDL_ClearError();
}

void close_window(SDL_Window* window) {
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void handle_events(SDL_Window* window) {
  bool quit = false;
  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit = true;
          break;
        default:
          break;
      }
    }
  }
  close_window(window);
}

int createSdlWindow(int windowWidth, int windowHeight) {
  const int kStencilBits = 8;  // Skia needs 8 stencil bits
                               // but it works with 0...
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, kStencilBits);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  // If you want multisampling, uncomment the below lines and set a sample count
  const int kMsaaSampleCount = 0;  // 4;
  // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, kMsaaSampleCount);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    handle_sdl_error();
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow(
      "SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowHeight,
      windowWidth, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    handle_sdl_error();
    return 1;
  }

  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    handle_sdl_error();
    return 1;
  }

  int result = SDL_GL_MakeCurrent(window, glContext);
  if (result != 0) {
    handle_sdl_error();
    return result;
  }

  // create window, create GL context, make it current

  int dw, dh;
  SDL_GL_GetDrawableSize(window, &dw, &dh);

  sk_sp<GrContext> grContext = GrContext::MakeGL();
  // setup GrContext
  // auto interface = GrGLMakeNativeInterface();
  // sk_sp<GrContext> grContext(GrContext::MakeGL(interface));
  // SkASSERT(grContext);

  // on linux both color types workds BGRA and RGBA
  SkColorType colorType = kRGBA_8888_SkColorType;
  GrGLenum colorFormat = GR_GL_RGBA8;  // linux open gl
  // colorParams.format = GR_GL_BGRA8; // try this on android

  GrGLint buffer;
  glGetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer);

  // create
  GrGLFramebufferInfo info;
  info.fFBOID = buffer;
  info.fFormat = colorFormat;

  GrBackendRenderTarget target(dw, dh, kMsaaSampleCount, kStencilBits, info);

  // setup SkSurface
  // To use distance field text, use commented out SkSurfaceProps instead
  // SkSurfaceProps props(SkSurfaceProps::kUseDeviceIndependentFonts_Flag,
  //                      SkSurfaceProps::kLegacyFontHost_InitType);
  SkSurfaceProps props(SkSurfaceProps::kLegacyFontHost_InitType);
  sk_sp<SkSurface> surface(SkSurface::MakeFromBackendRenderTarget(
      grContext.get(), target, kBottomLeft_GrSurfaceOrigin, colorType, nullptr,
      &props));

  SkCanvas* canvas = surface->getCanvas();
  // canvas->scale((float)dw/dm.w, (float)dh/dm.h);

  SkPaint paint;
  const char* message = "Hello world";
  paint.setColor(SK_ColorWHITE);
  canvas->drawText(message, strlen(message), SkIntToScalar(100),
                   SkIntToScalar(100), paint);

  canvas->flush();
  SDL_GL_SwapWindow(window);

  handle_events(window);
  return 0;
}

JSValueRef ObjectCallAsFunctionCallback(JSContextRef ctx, JSObjectRef function,
                                        JSObjectRef thisObject,
                                        size_t argumentCount,
                                        const JSValueRef arguments[],
                                        JSValueRef* exception) {
  std::cout << "Hello World" << std::endl;
  return JSValueMakeUndefined(ctx);
}

void js() {
  JSContextGroupRef contextGroup = JSContextGroupCreate();
  JSGlobalContextRef globalContext =
      JSGlobalContextCreateInGroup(contextGroup, nullptr);
  JSObjectRef globalObject = JSContextGetGlobalObject(globalContext);

  JSStringRef logFunctionName = JSStringCreateWithUTF8CString("log");
  JSObjectRef functionObject = JSObjectMakeFunctionWithCallback(
      globalContext, logFunctionName, &ObjectCallAsFunctionCallback);

  JSObjectSetProperty(globalContext, globalObject, logFunctionName,
                      functionObject, kJSPropertyAttributeNone, nullptr);

  JSStringRef logCallStatement = JSStringCreateWithUTF8CString("log()");

  JSEvaluateScript(globalContext, logCallStatement, nullptr, nullptr, 1,
                   nullptr);

  /* memory management code to prevent memory leaks */

  JSGlobalContextRelease(globalContext);
  JSContextGroupRelease(contextGroup);
  JSStringRelease(logFunctionName);
  JSStringRelease(logCallStatement);
}

int main() {
  auto name = "World";
  auto greeting = greet(name);

  std::cout << greeting;
  std::cout << std::endl;

  // js();

  int result = createSdlWindow(800, 600);
  if (result != 0) return result;
}
