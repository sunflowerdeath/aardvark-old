#include <iostream>
#include <memory>

#include "../utils/event_loop.hpp"
#include "../utils/websocket.hpp"

int main() {
    namespace beast = boost::beast;

    auto event_loop = aardvark::EventLoop();

    auto ws = std::make_shared<aardvark::Websocket>(event_loop.io, "localhost",
                                                    "9637");
    ws->start_signal.connect([](){
        std::cout << "start" << std::endl;
    });
    ws->close_signal.connect([](){
        std::cout << "close" << std::endl;
    });
    ws->message_signal.connect([](){
        std::cout << "message" << std::endl;
    });
    ws->error_signal.connect([](beast::error_code error){
        std::cout << "error" << std::endl;
    });
    ws->start();

    event_loop.run();
}
