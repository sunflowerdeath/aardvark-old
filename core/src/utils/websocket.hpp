#pragma once

#include <string>
#include <memory>
#include <nod/nod.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace aardvark {

namespace asio = boost::asio;
namespace beast = boost::beast;

enum class WebsocketState { connecting, open, closing, closed };

class Websocket : public std::enable_shared_from_this<Websocket> {
  public:
    Websocket(asio::io_context& io, std::string host, std::string port)
        : resolver(io), ws(io), host(host), port(port){};

    void open();
    void close();
    void send(std::string message);

    WebsocketState state;

    nod::signal<void()> open_signal;
    nod::signal<void(std::string)> error_signal;
    nod::signal<void(std::string)> message_signal;
    nod::signal<void()> close_signal;

  private:
    std::string host;
    std::string port;
    asio::ip::tcp::resolver resolver;
    beast::websocket::stream<beast::tcp_stream> ws;
    beast::flat_buffer buffer;

    void on_resolve(beast::error_code error,
                    asio::ip::tcp::resolver::results_type results);
    void on_connect(beast::error_code error,
                    asio::ip::tcp::resolver::results_type::endpoint_type);
    void on_handshake(beast::error_code error);
    void on_read(beast::error_code error, std::size_t bytes_transferred);
    void on_write(beast::error_code error, std::size_t bytes_transferred);
    void on_close(beast::error_code error);
};

}  // namespace aardvark
