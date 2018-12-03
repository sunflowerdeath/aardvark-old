#include "compositing.hpp"

#define GR_GL_FRAMEBUFFER_BINDING 0x8CA6
#define GR_GL_BGRA8 0x93A1
#define GR_GL_RGBA8 0x8058

namespace aardvark {

Layer::Layer(sk_sp<SkSurface> surface) {
  this->surface = surface;
  canvas = surface->getCanvas();
  auto image_info = canvas->imageInfo();
  size = Size{image_info.width(), image_info.height()};
};

void Layer::clear(SkColor color) {
  canvas->clear(color);
  set_changed();
};

void Layer::reset() {
  clear();
  compose_options = ComposeOptions();
};

void Layer::set_changed() {
  is_changed = true;
}

sk_sp<SkImage> Layer::get_snapshot() {
  if (is_changed) {
    is_changed = false;
    snapshot = surface->makeImageSnapshot();
  }
  return snapshot;
};

Compositor::Compositor(Size window_size) {
  this->window_size = window_size;
  gr_context = GrContext::MakeGL();
}

std::shared_ptr<Layer> Compositor::make_screen_layer() {
  const int stencil_bits = 8;
  const int msaa_sample_count = 0;

  // These values may be different on some devices
  const SkColorType color_type = kRGBA_8888_SkColorType;
  const GrGLenum color_format = GR_GL_RGBA8;

  // Wrap the frame buffer object attached to the screen in a Skia render target
  // Get an id of the currently bound framebuffer object
  GrGLint buffer;
  glGetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer);
  GrGLFramebufferInfo info;
  info.fFBOID = (GrGLuint) buffer;
  info.fFormat = color_format;

  // Create skia render target
  GrBackendRenderTarget target(window_size.width, window_size.height,
                               msaa_sample_count, stencil_bits, info);

  // Setup skia surface
  SkSurfaceProps props(SkSurfaceProps::kLegacyFontHost_InitType);
  auto surface(SkSurface::MakeFromBackendRenderTarget(
      gr_context.get(), target, kBottomLeft_GrSurfaceOrigin, color_type,
      nullptr, &props));
  return std::make_shared<Layer>(surface);
};

std::shared_ptr<Layer> Compositor::make_offscreen_layer(aardvark::Size size) {
  // sk_sp<GrContext> grContext(GrContext::MakeGL());
  const SkImageInfo info =
      SkImageInfo::MakeN32(size.width, size.height, kOpaque_SkAlphaType);
  auto surface(
      SkSurface::MakeRenderTarget(gr_context.get(), SkBudgeted::kNo, info));
  return std::make_shared<Layer>(surface);
};

void Compositor::paint_layer(Layer* target, Layer* layer, Position pos) {
  SkPaint paint;
  paint.setAlpha(128);
  auto res_pos = Position::add(layer->compose_options.translate, pos);
  target->canvas->drawImage(layer->get_snapshot(), res_pos.left, res_pos.top,
                            &paint);
};

} // namespace aardvark
