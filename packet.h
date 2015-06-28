#pragma once
#include <vector>
#include <boost/asio.hpp>

#include "key_value_store.h"

enum class OpCode : char { GET = 0x00, SET = 0x01 };

class Packet {

  typedef boost::asio::ip::tcp::socket socket;
  std::uint32_t readUInt32LE(char data[32], size_t offset);
  std::uint16_t readUInt16LE(char data[32], size_t offset);
  void writeUInt32LE(unsigned char* const data, std::uint32_t val);

public:
  Packet(char data[24], socket& socket,
         const std::shared_ptr<KeyValueStore>& k);
  void read(socket& socket);
  void respondToGet(socket& socket, const FlaggedValue& val, bool found);
  void respondToSet(socket& socket);
  static void printPacket(const char* const buf, const size_t len);
  static void printPacket(const std::vector<char>& buf);
  std::vector<char> key;
  std::vector<char> val;
  std::uint32_t bod_len;
  std::uint32_t opaque;
  std::uint32_t flags;
  std::uint16_t key_len;
  std::uint16_t reserved;
  char magic;
  char op;
  int ext_len;
  char dat_typ;
  static const int HEADER_LENGTH = 24;
  static const char MAGIC = 0x80;
};
