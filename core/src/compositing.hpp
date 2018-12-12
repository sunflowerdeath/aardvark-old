#pragma once

#include <memory>
#include <GL/gl.h>
#include "GrBackendSurface.h"
#include "GrContext.h"
#include "SkCanvas.h"
#include "SkSurface.h"
#include "base_types.hpp"

namespace aardvark {

struct ComposeOptions {
  Position translate;
  Scale scale;
  float opacity = 1;
  float rotation = 0;
};

class Layer {
 public:
  Layer(sk_sp<SkSurface> surface);

  Size size;
  ComposeOptions compose_options;
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

 private:
  bool is_changed = true;
  sk_sp<SkImage> snapshot;
};

class Compositor {
 public:
  Compositor(Size window_size);
  std::shared_ptr<Layer> make_screen_layer();
  std::shared_ptr<Layer> make_offscreen_layer(Size size);
  // Paints layer on top of the target layer
  void paint_layer(Layer* target, Layer* layer, Position pos);

 private:
  Size window_size;
  sk_sp<GrContext> gr_context;
};

};
