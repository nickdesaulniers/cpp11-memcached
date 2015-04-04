#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <boost/asio.hpp>

#include "packet.h"
#include "key_value_store.h"

using boost::asio::ip::tcp;

constexpr int port = 3000;
constexpr int header_length = 24;
constexpr char magic = 0x80;
constexpr char get_op = 0x00;
constexpr char set_op = 0x01;

void session (tcp::socket socket, const std::shared_ptr<KeyValueStore>& k) {
  boost::system::error_code ec;
  try {
    for (;;) {
      char data [header_length] = { 0 };
      size_t length = socket.read_some(boost::asio::buffer(data), ec);
      if (ec == boost::asio::error::eof) {
        std::cout << "end of client input" << std::endl;
        return;
      } else if (ec) {
        std::cout << "error reading from client" << std::endl;
        throw boost::system::system_error(ec);
      }
      if (data[0] != magic || length < header_length) {
        std::cout << "no magic byte or didn't read enough" << std::endl;
        //printf("%hhx : %zu\n", data[0], length);
        Packet::printPacket(data, length);
        return;
      }
      //Packet::printPacket(data, length);
      if (data[1] == get_op) {
        //std::cout << "get" << std::endl;
        auto p = Packet(data);
        p.read(socket);
        if (k->has(p.key)) {
          std::cout << "key found" << std::endl;
          std::vector<char> val = k->get(p.key);
          p.respondToGet(socket, val);
        } else {
          std::cout << "key not found" << std::endl;
        }
      } else if (data[1] == set_op) {
        //std::cout << "set" << std::endl;
        auto p = Packet(data);
        //std::cout << "created packet" << std::endl;
        p.read(socket);
        //std::cout << "p.read" << std::endl;
        k->set(p.key, p.val);
        //std::cout << "k->set" << std::endl;
        p.write(socket);
        //std::cout << "sent response" << std::endl;
      }
      //std::cout << "========================" << std::endl;
    }
  } catch (std::exception& e) {
    std::cerr << "Exception in thread: " << e.what() << std::endl;
  }
}

int main () {
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

