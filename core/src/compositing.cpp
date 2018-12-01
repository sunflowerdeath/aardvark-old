#include <memory>

#include <GL/gl.h>
#include "GrBackendSurface.h"
#include "GrContext.h"
#include "SkCanvas.h"
#include "SkSurface.h"

#include "compositing.hpp"

#define GR_GL_FRAMEBUFFER_BINDING 0x8CA6
#define GR_GL_BGRA8 0x93A1
#define GR_GL_RGBA8 0x8058

namespace aardvark::compositing {

Layer::Layer(sk_sp<SkSurface> surface) {
  this->surface = surface;
  canvas = surface->getCanvas();
};

void Layer::clear() {
  canvas->clear(SK_ColorBLACK);
};

void Layer::reset() {
};

Compositor::Compositor(Size windowSize) {
  this->windowSize = windowSize;
}

std::shared_ptr<Layer> Compositor::getScreenLayer() {
  const int kStencilBits = 8; // Skia needs 8 stencil bits
  const int kMsaaSampleCount = 0; //4

  // These values may be different on some devices
  const SkColorType colorType = kRGBA_8888_SkColorType;
  const GrGLenum colorFormat = GR_GL_RGBA8;

  // Setup GrContext
  sk_sp<GrContext> grContext(GrContext::MakeGL());

  // Wrap the frame buffer object attached to the screen in a Skia render target

  // Get currently bound framebuffer object id
  // This is probably redunant as buffer currently bound to screen
  // should be always 0
  GrGLint buffer;
  glGetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer);
  GrGLFramebufferInfo info;
  info.fFBOID = (GrGLuint) buffer; // Framebuffer object id
  info.fFormat = colorFormat;

  // Create skia render target
  GrBackendRenderTarget target(windowSize.width, windowSize.height,
                               kMsaaSampleCount, kStencilBits, info);

  // Setup skia surface
  SkSurfaceProps props(SkSurfaceProps::kLegacyFontHost_InitType);
  auto surface(SkSurface::MakeFromBackendRenderTarget(
      grContext.get(), target, kBottomLeft_GrSurfaceOrigin, colorType, nullptr,
      &props));
  return std::make_shared<Layer>(surface);
};

std::shared_ptr<Layer> Compositor::createOffscreenLayer(aardvark::Size size) {
  sk_sp<GrContext> grContext(GrContext::MakeGL());
  const SkImageInfo info =
      SkImageInfo::MakeN32(size.width, size.height, kOpaque_SkAlphaType);
  auto surface(
      SkSurface::MakeRenderTarget(grContext.get(), SkBudgeted::kNo, info));
  return std::make_shared<Layer>(surface);
};

void Compositor::paintLayer(Layer* layer, Position pos) {
};

} // namespace aardvark::compositing
