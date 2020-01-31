#include "element.hpp"

namespace aardvark {

class AndroidTextureElement : public Element {
  public:
    AndroidTextureElement(jobject surface_texture)
        : surface_texture(surface_texture),
          Element(/* is_repaint_boundary */ false,
                  /* size_depends_on_parent */ false) {
        controls_layer_tree = true;
    };
    std::string get_debug_name() override { return "AndroidTexture"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
    void new_frame() { has_new_frame = true; };
  private:
    jobject surface_texture;
    bool is_initialized = false;
    bool has_new_frame = false;
    AndroidTextureLayer layer;
};

}  // namespace aardvark


#include "android_texture_element.hpp"

namespace aardvark {

Size AndroidTextureElement::layout(BoxConstraints constraints) {
    return constraints.max_size();
}

void paint(bool is_changed) {
    if (is_initialized) return;
    layer = std::make_shared<AndroidTextureLayer>(document->gr_context,
                                                  surface_texture);
    layer_tree = std::make_shared<LayerTree>(this);
    layer_tree->add(layer);
    is_initialized = true;
}

}  // namespace aardvark
