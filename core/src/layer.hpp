#pragma once

#include <GL/gl.h>
#include <memory>
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
    void paint_layer(Layer* layer, Position pos);

    static std::shared_ptr<Layer> make_screen_layer(
        sk_sp<GrContext> gr_context);
    static std::shared_ptr<Layer> make_offscreen_layer(
        sk_sp<GrContext> gr_context, Size size);

  private:
    bool is_changed = true;
    sk_sp<SkImage> snapshot;
};

};  // namespace aardvark
