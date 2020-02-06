#pragma once

class BaseClass {
  public:
    virtual ~BaseClass() {};
    int base_method() { return 25; }
};

class SuperClass : public BaseClass {};
