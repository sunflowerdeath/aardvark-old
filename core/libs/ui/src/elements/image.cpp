#include "elements/image.hpp"

namespace aardvark {

std::pair<Position, Size> fit_image(
    Size bounds, Size img_size, ImageFit fit, Size custom_size) {
    auto width = 0.0f;
    auto height = 0.0f;
    switch (fit) {
        case ImageFit::none:
            width = img_size.width;
            height = img_size.height;
            break;
        case ImageFit::custom_size:
            width = custom_size.width;
            height = custom_size.height;
            break;
        case ImageFit::cover: {
            auto width_ratio = img_size.width / bounds.width;
            auto height_ratio = img_size.height / bounds.height;
            auto ratio = fmin(width_ratio, height_ratio);
            width = img_size.width / ratio;
            height = img_size.height / ratio;
        } break;
        case ImageFit::contain: {
            auto width_ratio = img_size.width / bounds.width;
            auto height_ratio = img_size.height / bounds.height;
            auto ratio = fmax(width_ratio, height_ratio);
            width = img_size.width / ratio;
            height = img_size.height / ratio;
        } break;
        case ImageFit::fill:
            width = bounds.width;
            height = bounds.height;
            break;
        case ImageFit::scale_down:
            if (img_size.width > bounds.width ||
                img_size.height > bounds.height) {
                auto width_ratio = img_size.width / bounds.width;
                auto height_ratio = img_size.height / bounds.height;
                auto ratio = fmax(width_ratio, height_ratio);
                width = img_size.width / ratio;
                height = img_size.height / ratio;
            } else {
                width = img_size.width;
                height = img_size.height;
            }
            break;
    }
    auto left = (bounds.width - width) / 2;
    auto top = (bounds.height - height) / 2;
    return std::make_pair(Position{left, top}, Size{width, height});
}

void ImageElement::init_image() {
    data = src->get_data();
    auto sk_data = SkData::MakeWithoutCopy(data.data(), data.size());
    image = SkImage::MakeFromEncoded(sk_data);
}

void ImageElement::paint(bool is_changed) {
    if (image == nullptr) init_image();
    auto [fit_pos, fit_size] = fit_image(
        size,                                                 // bounds
        Size{(float)image->width(), (float)image->height()},  // img_size
        fit,                                                  // fit
        custom_size                                           // custom_size
    );
    auto layer = document->get_layer();
    document->setup_layer(layer, this);
    auto paint = SkPaint();
    layer->canvas->drawImageRect(
        image,
        SkRect::MakeXYWH(
            fit_pos.left, fit_pos.top, fit_size.width, fit_size.height),
        &paint);
}

void SvgImageElement::init_svg() {
    auto data = src->get_data();
    svg = std::unique_ptr<SVGNative::SVGDocument>(
        SVGNative::SVGDocument::CreateSVGDocument(data.c_str(), renderer));
}

void SvgImageElement::paint(bool is_changed) {
    if (svg == nullptr) init_svg();
    auto img_size = Size{(float)svg->Width(), (float)svg->Height()};
    auto [fit_pos, fit_size] = fit_image(
        size,        // bounds
        img_size,    // img_size
        fit,         // fit
        custom_size  // custom_size
    );
    auto layer = document->get_layer();
    document->setup_layer(layer, this);
    layer->canvas->save();
    if (fit_pos != Position::origin) {
        layer->canvas->translate(fit_pos.left, fit_pos.top);
    }
    if (fit_size != img_size) {
        layer->canvas->scale(
            fit_size.width / img_size.width, fit_size.height / img_size.height);
    }
    renderer->SetSkCanvas(layer->canvas);
    svg->Render(img_size.width, img_size.height);
    layer->canvas->restore();
}

}  // namespace aardvark
