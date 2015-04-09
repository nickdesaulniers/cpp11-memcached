#include "packet.h"

Packet::Packet(char data[24], tcp::socket& socket,
               const std::shared_ptr<KeyValueStore>& k)
    : bod_len(readUInt32LE(data, 8)),
      opaque(readUInt32LE(data, 12)),
      key_len(readUInt16LE(data, 2)),
      op(data[1]),
      ext_len(data[4]) {
  read(socket);
  if (data[1] == static_cast<char>(OpCode::GET)) {
    if (k->has(key)) {
      respondToGet(socket, k->get(key), true);
    } else {
      std::string msg = "Key not found";
      std::vector<char> val(msg.cbegin(), msg.cend());
      respondToGet(socket, FlaggedValue{ val, 0 }, false);
    }
  } else if (data[1] == static_cast<char>(OpCode::SET)) {
    k->set(key, FlaggedValue{ val, flags });
    respondToSet(socket);
  }
}

void Packet::read(tcp::socket& socket) {
  uint32_t bytes_read = 0;

  if (ext_len > 0) {
    std::vector<char> extras(ext_len);
    while (bytes_read < static_cast<uint32_t>(ext_len)) {
      bytes_read += socket.receive(boost::asio::buffer(extras));
    }
    bytes_read = 0;

    if (extras.size() > 4) {
      char flags_buf[4] = { extras[0], extras[1], extras[2], extras[3] };
      flags = readUInt32LE(flags_buf, 0);
    }
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
void Packet::respondToSet(tcp::socket& socket) {
  unsigned char res[24] = { 0x81, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
  writeUInt32LE(&res[12], opaque);
  try {
    boost::asio::write(socket, boost::asio::buffer(res));
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

// MUST have extras.
// MAY have key.
// MAY have value.
void Packet::respondToGet(tcp::socket& socket, const FlaggedValue& fv,
                          bool found) {
  unsigned char res[28] = { 0x81, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 };
  writeUInt32LE(&res[8], fv.value.size() + 4);
  writeUInt32LE(&res[12], opaque);
  writeUInt32LE(&res[24], fv.flags);
  // Section 4.1:
  // If the status code of a response packet is non-nil, the body of the packet
  // will contain a textual error message.
  if (!found) {
    res[7] = 0x01;
  }
  try {
    boost::asio::write(socket, boost::asio::buffer(res));
    boost::asio::write(socket, boost::asio::buffer(fv.value));
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

// x86-64 is little endian, but this won't work elsewhere...
uint32_t Packet::readUInt32LE(char data[32], size_t offset) {
  return htonl(*reinterpret_cast<uint32_t*>(&data[offset]));
}

uint16_t Packet::readUInt16LE(char data[32], size_t offset) {
  return htons(*reinterpret_cast<uint16_t*>(&data[offset]));
}

void Packet::writeUInt32LE(unsigned char* const data, uint32_t val) {
  data[0] = (val & 0xFF000000) >> 24;
  data[1] = (val & 0x00FF0000) >> 16;
  data[2] = (val & 0x0000FF00) >> 8;
  data[3] = val & 0x000000FF;
}

void Packet::printPacket(const char* const buf, const size_t len) {
  for (size_t i = 0; i < len; ++i) {
    if (i % 8 == 0) {
      std::cout << std::endl;
    }
    printf("%02hhx|", buf[i]);
  }
  std::cout << std::endl;
}

void Packet::printPacket(const std::vector<char>& buf) {
  for (size_t i = 0; i < buf.size(); ++i) {
    if (i % 8 == 0) {
      std::cout << std::endl;
    }
    printf("%02hhx|", buf[i]);
  }
  std::cout << std::endl;
}
