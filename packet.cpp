#include "packet.h"

void Packet::read (tcp::socket& socket) {
  uint32_t bytes_read = 0;

  //std::cout << "packet.read, ext_len: " << ext_len << ", key_len: " << key_len
    //<< ", bod_len: " << bod_len << std::endl;
  if (ext_len > 0) {
    std::vector<char> garbage (ext_len);
    while (bytes_read < static_cast<uint32_t>(ext_len)) {
      bytes_read += socket.receive(boost::asio::buffer(garbage));
    }
    //std::cout << "read ext bytes: " << ext_len << std::endl;
    bytes_read = 0;
  }

  if (key_len > 0) {
    //std::cout << "going to read key" << std::endl;
    key.resize(key_len);
    while (bytes_read < key_len) {
      bytes_read += socket.receive(boost::asio::buffer(key));
    }

    //std::cout << "read " << bytes_read << "B, key.size(): " << key.size() << std::endl;
    //std::cout << "key ";
    //for (char& x : key) {
      //printf("%02hhx|", x);
    //}
    //std::cout << std::endl;
    bytes_read = 0;
  }

  //std::cout << "bod_len: " << bod_len << std::endl;
  //std::cout << "key_len: " << key_len << std::endl;
  if (bod_len > key_len) {
    //std::cout << "going to read val" << std::endl;
    uint32_t val_len = bod_len - key_len - ext_len;
    //std::cout << "val_len: " << val_len << std::endl;
    val.resize(val_len);
    while (bytes_read < val_len) {
      bytes_read += socket.receive(boost::asio::buffer(val));
    }

    //std::cout << "val ";
    //for (char& x : val) {
      //printf("%02hhx|", x);
    //}
    //std::cout << std::endl;
    //std::cout << "read " << bytes_read << "B, val.size(): " << val.size() << std::endl;
  }
}

void Packet::write (tcp::socket& socket) {
  size_t bytes_sent = 0;
  char res [24] = {
    // yuck
    static_cast<char>(0x81), 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, // opaque
    //opaque[0], opaque[1], opaque[2], opaque[3],
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01
  };
  writeUInt32LE(&res[12], opaque);
  //std::cout << val.size() << std::endl;
  //std::cout << "--Response--" << std::endl;
  //Packet::printPacket(res, 24);
  // yuck
  try {
    bytes_sent += boost::asio::write(socket, boost::asio::buffer(res));
    //std::cout << "sent bytes: " << bytes_sent << std::endl;
  } catch (std::exception& e) { std::cout << e.what() << std::endl; }
}

// Must have extras.
// May have key.
// May have value.
void Packet::respondToGet (tcp::socket& socket, const std::vector<char>& val) {
  size_t bytes_sent = 0;
  //std::cout << "----------" << std::endl;
  char res [28] = {
    static_cast<char>(0x81), 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, // this needs to be length of body
    0x00, 0x00, 0x00, 0x00, // opaque
    //opaque[0], opaque[1], opaque[2], opaque[3],
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00
  };
  //Packet::printPacket(res, 28);
  //std::cout << "----------" << std::endl;
  // 4 from 4B in extras that's counted as body size
  writeUInt32LE(&res[8], val.size() + 4);
  writeUInt32LE(&res[12], opaque);
  //std::cout << "--Response--" << std::endl;
  //Packet::printPacket(res, 28);
  //std::cout << "----------" << std::endl;
  //std::cout << val.size() << std::endl;
  // write the header
  try {
    bytes_sent += boost::asio::write(socket, boost::asio::buffer(res));
  } catch (std::exception& e) { std::cerr << e.what() << std::endl; }
  //std::cout << "sent header-bytes: " << bytes_sent << std::endl;
  bytes_sent = 0;
  //std::cout << "val.size(): " << val.size() << std::endl;
  //Packet::printPacket(val);
  bytes_sent += socket.write_some(boost::asio::buffer(val));
  //while (bytes_sent < val.size()) {
    //bytes_sent += boost::asio::write(socket, boost::asio::buffer(val));
  //}
  //std::cout << "sent body-bytes: " << bytes_sent << std::endl;
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

