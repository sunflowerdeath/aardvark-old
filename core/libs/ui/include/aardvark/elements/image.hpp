#pragma once

#include "../base_types.hpp"
#include "../element.hpp"
#include "../utils/files_utils.hpp"

namespace aardvark {

class DataSource {
  public:
    virtual std::string get_data() = 0;
};

class FileDataSource : public DataSource {
  public:
    FileDataSource(std::string path) : path(std::move(path)){};

    std::string get_data() override {
        return utils::read_text_file(path);
    };

  private:
    std::string path;
};

class ImageElement : public Element {
  public:
    ImageElement(std::shared_ptr<DataSource> src = nullptr)
        : src(std::move(src)),
          Element(
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    std::string get_debug_name() override { return "Image"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

    void set_src(std::shared_ptr<DataSource> src) {
        this->src = std::move(src);
        data.clear();
        image = nullptr;
    }

    std::shared_ptr<DataSource> src = nullptr;

  private:
    void init_image();
    std::string data = "";
    sk_sp<SkImage> image = nullptr;
};

}  // namespace aardvark
