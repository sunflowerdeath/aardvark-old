#include "elements/image.hpp"

namespace aardvark {

void ImageElement::init_image() {
    data = src->get_data();
    auto sk_data = SkData::MakeWithoutCopy(data.data(), data.size());
    image = SkImage::MakeFromEncoded(sk_data);
}

Size ImageElement::layout(BoxConstraints constraints) {
    return constraints.max_size();
}

void ImageElement::paint(bool is_changed) {
    if (image == nullptr) init_image();

    auto layer = document->get_layer();
    document->setup_layer(layer, this);

    auto img_width = image->width();
    auto img_height = image->height();
    auto width = 0.0;
    auto height = 0.0;
    switch (fit) {
        case ImageFit::size:
            width = size.width;
            height = size.height;
            break;
        case ImageFit::none:
            width = img_width;
            height = img_height;
            break;
        case ImageFit::cover: {
            auto width_ratio = img_width / size.width;
            auto height_ratio = img_height / size.height;
            auto ratio = fmin(width_ratio, height_ratio);
            width = img_width / ratio;
            height = img_height / ratio;
        } break;
        case ImageFit::contain: {
            auto width_ratio = img_width / size.width;
            auto height_ratio = img_height / size.height;
            auto ratio = fmax(width_ratio, height_ratio);
            width = img_width / ratio;
            height = img_height / ratio;
        } break;
        case ImageFit::fill:
            width = size.width;
            height = size.height;
            break;
        case ImageFit::scale_down:
            if (img_width > size.width || img_height > size.height) {
                auto width_ratio = img_width / size.width;
                auto height_ratio = img_height / size.height;
                auto ratio = fmax(width_ratio, height_ratio);
                width = img_width / ratio;
                height = img_height / ratio;
            } else {
                width = img_width;
                height = img_height;
            }
            break;
    }

    auto left = (size.width - width) / 2;;
    auto top = (size.height - height) / 2;;

    auto paint = SkPaint();
    layer->canvas->drawImageRect(
        image,
        SkRect::MakeXYWH(0, 0, image->width(), image->height()),
        SkRect::MakeXYWH(left, top, width, height),
        &paint);
}

}  // namespace aardvark
