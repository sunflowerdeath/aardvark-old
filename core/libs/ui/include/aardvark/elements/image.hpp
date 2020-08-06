#pragma once

#include <svgnative/SVGDocument.h>
#include <svgnative/ports/skia/SkiaSVGRenderer.h>

#include "../base_types.hpp"
#include "../element.hpp"
#include "../utils/data_source.hpp"

namespace aardvark {

enum class ImageFit {
    // Image scaled to fit, maintains original ratio
    contain,
    // Image sized to fill entire box, maintains original ratio
    cover,
    // Image scaled down to fit box if necessary 
    scale_down,
    // Resize image to fill box, can change image ratio
    fill,
    // Do not resize image
    none,
    // Size image using the `customSize` property
    custom_size
};

class ImageElement : public Element {
  public:
    ImageElement(
        std::shared_ptr<DataSource> src = nullptr,
        ImageFit fit = ImageFit::none)
        : src(std::move(src)),
          fit(fit),
          Element(
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ true){};

    std::string get_debug_name() override { return "Image"; };
    Size layout(BoxConstraints constraints) override {
        return constraints.max_size();
    };
    void paint(bool is_changed) override;

    std::shared_ptr<DataSource> src = nullptr;
    void set_src(std::shared_ptr<DataSource> src) {
        this->src = std::move(src);
        data.clear();
        image = nullptr;
    }

    ELEMENT_PROP_DEFAULT(ImageFit, fit, ImageFit::none);
    ELEMENT_PROP(Size, custom_size);

  private:
    void init_image();
    std::string data = "";
    sk_sp<SkImage> image = nullptr;
};

using ColorMap = std::unordered_map<std::string, Color>;

class SvgImageElement : public Element {
  public:
    SvgImageElement(
        std::shared_ptr<DataSource> src = nullptr,
        ImageFit fit = ImageFit::none)
        : src(std::move(src)),
          fit(fit),
          renderer(std::make_shared<SVGNative::SkiaSVGRenderer>()),
          Element(
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ true){};

    std::string get_debug_name() override { return "SvgImage"; };
    Size layout(BoxConstraints constraints) override {
        return constraints.max_size();
    };
    void paint(bool is_changed) override;

    std::shared_ptr<DataSource> src = nullptr;
    void set_src(std::shared_ptr<DataSource> src) {
        this->src = std::move(src);
        svg = nullptr;
    }

    ELEMENT_PROP_DEFAULT(ImageFit, fit, ImageFit::none);
    ELEMENT_PROP(Size, custom_size);
    ELEMENT_PROP_DEFAULT(ColorMap, color_map, {});

  private:
    void init_svg();
    std::shared_ptr<SVGNative::SkiaSVGRenderer> renderer;
    std::unique_ptr<SVGNative::SVGDocument> svg;
};

}  // namespace aardvark
