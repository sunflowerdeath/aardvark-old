#include "timers_host.hpp"

#include <algorithm>

namespace aardvark::js {

int TimersHost::set_timeout(JSValueRef function, int delay) {
    key++;
    auto timer = &(timers.emplace(key, Timer()).first->second);
    timer->function = JSValueToObject(ctx, function, nullptr);
    timer->key = key;
    timer->thread = std::thread([this, timer, delay]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        expired_timers.push_back(timer);
    });
    timer->thread.detach();
    return key;
};

void TimersHost::clear_timeout(int id) {
    if (timers.find(id) == timers.end()) return;
    auto it =
        std::find(expired_timers.begin(), expired_timers.end(), &timers[id]);
    if (it != expired_timers.end()) expired_timers.erase(it);
    timers.erase(id);
};

void TimersHost::call_expired() {
    for (auto& timer : expired_timers) {
        JSObjectCallAsFunction(ctx,              // ctx
                               timer->function,  // object
                               nullptr,          // thisObject
                               0,                // argumentCount
                               nullptr,          // arguments[],
                               nullptr           // exception
        );
        timers.erase(timers.find(timer->key));
    }
    expired_timers.clear();
};

}  // namespace aardvark::js
