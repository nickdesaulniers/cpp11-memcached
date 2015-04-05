#include "packet.h"

void Packet::read (tcp::socket& socket) {
  uint32_t bytes_read = 0;

  if (ext_len > 0) {
    std::vector<char> garbage (ext_len);
    while (bytes_read < static_cast<uint32_t>(ext_len)) {
      bytes_read += socket.receive(boost::asio::buffer(garbage));
    }
    bytes_read = 0;
  }

  if (key_len > 0) {
    key.resize(key_len);
    while (bytes_read < key_len) {
      bytes_read += socket.receive(boost::asio::buffer(key));
    }
    bytes_read = 0;
  }

  if (bod_len > key_len) {
    uint32_t val_len = bod_len - key_len - ext_len;
    val.resize(val_len);
    while (bytes_read < val_len) {
      bytes_read += socket.receive(boost::asio::buffer(val));
    }
  }
}

// Must have extras.
// May have key.
// May have value.
void Packet::respondToSet (tcp::socket& socket) {
  size_t bytes_sent = 0;
  char res [24] = {
    static_cast<char>(0x81), 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01
  };
  writeUInt32LE(&res[12], opaque);
  try {
    bytes_sent += boost::asio::write(socket, boost::asio::buffer(res));
  } catch (std::exception& e) { std::cout << e.what() << std::endl; }
}

// MUST have extras.
// MAY have key.
// MAY have value.
void Packet::respondToGet (tcp::socket& socket, const std::vector<char>& val) {
  size_t bytes_sent = 0;
  char res [28] = {
    static_cast<char>(0x81), 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00
  };
  writeUInt32LE(&res[8], val.size() + 4);
  writeUInt32LE(&res[12], opaque);
  // write the header
  try {
    bytes_sent += boost::asio::write(socket, boost::asio::buffer(res));
  } catch (std::exception& e) { std::cerr << e.what() << std::endl; }
  bytes_sent = 0;
  bytes_sent += socket.write_some(boost::asio::buffer(val));
}

// x86-64 is little endian, but this won't work elsewhere...
uint32_t Packet::readUInt32LE (char data [32], size_t offset) {
  return htonl(*reinterpret_cast<uint32_t*>(&data[offset]));
}

uint16_t Packet::readUInt16LE (char data [32], size_t offset) {
  return htons(*reinterpret_cast<uint16_t*>(&data[offset]));
}

void Packet::writeUInt32LE (char* const data, uint32_t val) {
  data[0] = val & 0xFF000000;
  data[1] = val & 0x00FF0000;
  data[2] = val & 0x0000FF00;
  data[3] = val & 0x000000FF;
}

void Packet::printPacket (const char* const buf, const size_t len) {
  for (size_t i = 0; i < len; ++i) {
    if (i % 8 == 0) {
      std::cout << std::endl;
    }
    printf("%02hhx|", buf[i]);
  }
  std::cout << std::endl;
}

void Packet::printPacket (const std::vector<char>& buf) {
  for (size_t i = 0; i < buf.size(); ++i) {
    if (i % 8 == 0) {
      std::cout << std::endl;
    }
    printf("%02hhx|", buf[i]);
  }
  std::cout << std::endl;
}

