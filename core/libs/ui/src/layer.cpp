#include "layer.hpp"

#include <cstdlib>
#include "utils/log.hpp"

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

void Layer::set_changed() { is_changed = true; }

sk_sp<SkImage> Layer::get_snapshot() {
    if (is_changed) {
        is_changed = false;
        snapshot = surface->makeImageSnapshot();
    }
    return snapshot;
};

void Layer::paint_layer(Layer* layer, Position pos, float opacity) {
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setAlpha(opacity * 255);
    auto sampling = SkSamplingOptions(SkFilterMode::kNearest, SkMipmapMode::kNone);
    canvas->drawImage(layer->get_snapshot(), pos.left, pos.top, sampling, &paint);
};

const int STENCIL_BITS = 8;
const int MSAA_SAMPLE_COUNT = 0;

void measure_current_viewport_size(GLint* width, GLint* height) {
    GLint dimensions[4];
    glGetIntegerv(GL_VIEWPORT, dimensions);
    *width = dimensions[2];
    *height = dimensions[3];
}

std::shared_ptr<Layer> Layer::make_screen_layer(sk_sp<GrDirectContext> gr_context) {
    // These values may be different on some devices
    const SkColorType color_type = kRGBA_8888_SkColorType;
    const GrGLenum color_format = GR_GL_RGBA8;

    // Measure viewport size
    GLint width;
    GLint height;
    measure_current_viewport_size(&width, &height);

    // Get an id of the currently attached to the screen framebuffer
    GrGLint buffer;
    glGetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer);

    // Wrap the framebuffer in a Skia render target
    GrGLFramebufferInfo info;
    info.fFBOID = (GrGLuint)buffer;
    info.fFormat = color_format;
    auto target = GrBackendRenderTarget(width, height, MSAA_SAMPLE_COUNT,
                                        STENCIL_BITS, info);

    // Setup skia surface
    auto props = SkSurfaceProps(SkSurfaceProps::kUseDeviceIndependentFonts_Flag,
      kUnknown_SkPixelGeometry);
    auto surface = SkSurface::MakeFromBackendRenderTarget(
        gr_context.get(), // context,
        target, // backendRenderTarget,
        kBottomLeft_GrSurfaceOrigin, // origin,
        color_type, // colorType,
        nullptr, // colorSpace,
        &props // surfaceProps
    );
	if (surface == nullptr) {
		Log::error("[Layer] Cannot create screen layer surface");
	}

    return std::make_shared<Layer>(surface);
};

std::shared_ptr<Layer> Layer::make_offscreen_layer(sk_sp<GrDirectContext> gr_context,
                                                   Size size) {
    const SkImageInfo info =
        SkImageInfo::MakeN32Premul(size.width, size.height);
    auto props = SkSurfaceProps(SkSurfaceProps::kUseDeviceIndependentFonts_Flag,
      kUnknown_SkPixelGeometry);
    auto target =
        SkSurface::MakeRenderTarget(
          gr_context.get(),
          SkBudgeted::kNo,
          info,
          MSAA_SAMPLE_COUNT, // sampleCount
          &props
        );
    if (target == nullptr) {
		Log::error("[Layer] Cannot create offscreen layer render target");
    }
    auto surface(target);
    if (surface == nullptr) {
		Log::error("[Layer] Cannot create offscreen layer surface");
    }
    return std::make_shared<Layer>(surface);
};

}  // namespace aardvark
