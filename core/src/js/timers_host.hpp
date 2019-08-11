#pragma once

#include <vector>
#include <unordered_map>
#include <thread>

#include "JavaScriptCore/JavaScript.h"

namespace aardvark::js {

struct Timer {
    std::thread thread;
    int key;
    JSObjectRef function;
};

class TimersHost {
  public:
    TimersHost(){};
    int set_timeout(JSValueRef function, int delay);
    void clear_timeout(int id);
    void call_expired();
    JSContextRef ctx;

  private:
    int key = 0;
    std::unordered_map<int, Timer> timers;
    std::vector<Timer*> expired_timers;
};

}
