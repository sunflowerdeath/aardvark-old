#pragma once

#include <memory>
#ifdef ADV_PLATFORM_ANDROID
#include <GLES/gl.h>
#endif
#ifdef ADV_PLATFORM_DESKTOP
// #include <GL/gl.h>
#include <OpenGL/gl.h>
#endif
#include "GrDirectContext.h"
#include "GrBackendSurface.h"
#include "SkCanvas.h"
#include "SkSurface.h"
#include "base_types.hpp"

namespace aardvark {

class Layer {
  public:
    Layer(sk_sp<SkSurface> surface);

    Size size;
    sk_sp<SkSurface> surface;
    SkCanvas* canvas;

    // Clears layer's canvas
    void clear(SkColor color = SK_ColorTRANSPARENT);

    // Clears layer and resets compose options to default
    void reset(SkColor color = SK_ColorTRANSPARENT);

    // Mark layer as changed to invalidate last snapshot
    void set_changed();

    // Returns snapshot of the layer's canvas
    sk_sp<SkImage> get_snapshot();

    // Paints layer on top of this layer
    void paint_layer(Layer* layer, Position pos, float opacity = 1);

    static std::shared_ptr<Layer> make_screen_layer(
        sk_sp<GrDirectContext> gr_context);
    static std::shared_ptr<Layer> make_offscreen_layer(
        sk_sp<GrDirectContext> gr_context, Size size);

  private:
    bool is_changed = true;
    sk_sp<SkImage> snapshot;
};

}  // namespace aardvark
