#include <thread>
#include <boost/asio.hpp>

#include "packet.h"

using boost::asio::ip::tcp;

const int port = 3000;

void session (tcp::socket socket, const std::shared_ptr<KeyValueStore>& k) {
  boost::system::error_code ec;
  try {
    for (;;) {
      char data [Packet::HEADER_LENGTH] = { 0 };
      size_t bytes_read = socket.read_some(boost::asio::buffer(data), ec);
      if (ec == boost::asio::error::eof) {
        return;
      } else if (ec) {
        throw boost::system::system_error(ec);
      } else if (data[0] != Packet::MAGIC || bytes_read < Packet::HEADER_LENGTH) {
        return;
      }
      Packet(data, socket, k);
    }
  } catch (std::exception& e) {
    std::cerr << "Exception in thread: " << e.what() << std::endl;
  }
}

bool isBigEndian () {
  int i = 1;
  return (*(char*)&i) == 0;
}

int main () {
  if (isBigEndian()) {
    std::cerr << "Parsing code not portable to big endian hosts, yet" << std::endl;
    return 1;
  }
  auto k = std::make_shared<KeyValueStore>();
  try {
    boost::asio::io_service io_service;
    tcp::acceptor acceptor (io_service, tcp::endpoint(tcp::v4(), 3000));
    std::cout << "listening on port: " << port << std::endl;

    for (;;) {
      tcp::socket socket (io_service);
      acceptor.accept(socket);
      std::thread(session, std::move(socket), k).detach();
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

