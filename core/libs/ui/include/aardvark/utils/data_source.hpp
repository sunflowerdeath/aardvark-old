#pragma once

#include <string>

#include "files_utils.hpp"

namespace aardvark {

class DataSource {
  public:
    virtual std::string get_data() = 0;
};

class File : public DataSource {
  public:
    File(std::string path) : path(std::move(path)){};

    std::string get_data() override {
        return utils::read_text_file(path);
    };

  private:
    std::string path;
};

class StringSrc : public DataSource {
  public:
    StringSrc(std::string data) : data(std::move(data)){};

    std::string get_data() override { return data; };

  private:
    std::string data;
};

}
