#pragma once

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
  void clear();
  void reset();
};

class Compositor {
 public:
  Compositor(Size windowSize);
  std::shared_ptr<Layer> getScreenLayer();
  std::shared_ptr<Layer> createOffscreenLayer(Size size);
  void paintLayer(Layer* layer, Position pos);
 private:
  Size windowSize;
};

};
