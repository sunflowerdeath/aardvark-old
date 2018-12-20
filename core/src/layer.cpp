#include "layer.hpp"

#define GR_GL_FRAMEBUFFER_BINDING 0x8CA6
#define GR_GL_BGRA8 0x93A1
#define GR_GL_RGBA8 0x8058

namespace aardvark {

Layer::Layer(sk_sp<SkSurface> surface) {
  this->surface = surface;
  canvas = surface->getCanvas();
  auto image_info = canvas->imageInfo();
  size = Size{static_cast<float>(image_info.width()),
              static_cast<float>(image_info.height())};
};

void Layer::clear(SkColor color) {
  canvas->clear(color);
  set_changed();
};

void Layer::reset(SkColor color) {
  canvas->restoreToCount(1);
  clear(color);
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

void Layer::paint_layer(Layer* layer, Position pos) {
  // auto res_pos = Position::add(layer->compose_options.translate, pos);
  SkPaint paint;
  paint.setAntiAlias(true);
  canvas->drawImage(layer->get_snapshot(), pos.left, pos.top, &paint);
};

const int STENCIL_BITS = 8;
const int MSAA_SAMPLE_COUNT = 4;

std::shared_ptr<Layer> Layer::make_screen_layer(sk_sp<GrContext> gr_context) {
  // sk_sp<GrContext> gr_context = GrContext::MakeGL();
  // These values may be different on some devices
  const SkColorType color_type = kRGBA_8888_SkColorType;
  const GrGLenum color_format = GR_GL_RGBA8;
  // Wrap the frame buffer object attached to the screen in a Skia render target
  // Get an id of the current framebuffer object
  GrGLint buffer;
  glGetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer);
  GrGLFramebufferInfo info;
  info.fFBOID = (GrGLuint) buffer;
  info.fFormat = color_format;
  // Create skia render target
  GLint dimensions[4];;
  glGetIntegerv(GL_VIEWPORT, dimensions);
  GLint width = dimensions[2];
  GLint height = dimensions[3];
  GrBackendRenderTarget target(width, height, MSAA_SAMPLE_COUNT, STENCIL_BITS,
                               info);
  // Setup skia surface
  SkSurfaceProps props(SkSurfaceProps::kLegacyFontHost_InitType);
  auto surface(SkSurface::MakeFromBackendRenderTarget(
      gr_context.get(), target, kBottomLeft_GrSurfaceOrigin, color_type,
      nullptr, &props));
  return std::make_shared<Layer>(surface);
};

std::shared_ptr<Layer> Layer::make_offscreen_layer(sk_sp<GrContext> gr_context,
                                                   Size size) {
  const SkImageInfo info = SkImageInfo::MakeN32Premul(size.width, size.height);
  auto surface(
      SkSurface::MakeRenderTarget(gr_context.get(), SkBudgeted::kNo, info));
  return std::make_shared<Layer>(surface);
};

} // namespace aardvark
