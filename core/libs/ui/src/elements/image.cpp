#include "elements/image.hpp"

namespace aardvark {

void ImageElement::init_image() {
    data = src->get_data();
    auto sk_data = SkData::MakeWithoutCopy(data.data(), data.size());
    image = SkImage::MakeFromEncoded(sk_data);
}

Size ImageElement::layout(BoxConstraints constraints) {
    if (image == nullptr) init_image();
    return Size{
        fmin(image->width(), constraints.max_width), // width
        fmin(image->height(), constraints.max_height), // height
    };
}

void ImageElement::paint(bool is_changed) {
    auto layer = document->get_layer();
    document->setup_layer(layer, this);
    layer->canvas->drawImage(image, 0, 0); // TODO respect constraints?
}

}  // namespace aardvark
