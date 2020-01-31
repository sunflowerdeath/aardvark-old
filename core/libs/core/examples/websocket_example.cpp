#include <iostream>
#include <memory>

#include "../utils/event_loop.hpp"
#include "../utils/websocket.hpp"

int main() {
    auto event_loop = aardvark::EventLoop();
    auto ws = std::make_shared<aardvark::Websocket>(event_loop.io,
                                                    "echo.websocket.org", "80");
    ws->open_signal.connect([ws](){
        std::cout << "open" << std::endl;
        ws->send("Hello");
    });
    ws->close_signal.connect([](){
        std::cout << "close" << std::endl;
    });
    ws->message_signal.connect([](std::string message){
        std::cout << "message: " << message << std::endl;
    });
    ws->error_signal.connect([](std::string error){
        std::cout << "error: " << error << std::endl;
    });
    ws->open();
    event_loop.run();
}
