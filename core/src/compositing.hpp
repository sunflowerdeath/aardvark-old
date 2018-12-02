#pragma once

#include <memory>
#include <GL/gl.h>
#include "GrBackendSurface.h"
#include "GrContext.h"
#include "SkCanvas.h"
#include "SkSurface.h"
#include "base_types.hpp"

namespace aardvark::compositing {

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
  void clear(SkColor color = SK_ColorBLACK);
  void reset();
  void set_changed();
  sk_sp<SkImage> get_snapshot();
 private:
  sk_sp<SkImage> snapshot;
};

class Compositor {
 public:
  Compositor(Size window_size);
  std::shared_ptr<Layer> get_screen_layer();
  std::shared_ptr<Layer> create_offscreen_layer(Size size);
  void paint_layer(Layer* screen, Layer* layer, Position pos);
 private:
  Size window_size;
  sk_sp<GrContext> gr_context;
};

};
