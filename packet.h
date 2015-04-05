#pragma once
#include <vector>
#include <boost/asio.hpp>

#include "key_value_store.h"

using boost::asio::ip::tcp;

enum class OpCode : char { GET = 0x00, SET = 0x01 };

class Packet {
public:
  Packet(char data[24], tcp::socket& socket,
         const std::shared_ptr<KeyValueStore>& k);
  void read(tcp::socket& socket);
  void respondToGet(tcp::socket& socket, const std::vector<char>& val,
                    bool found);
  void respondToSet(tcp::socket& socket);
  void set(const std::vector<char>& key, const std::vector<char>& val);
  static void printPacket(const char* const buf, const size_t len);
  static void printPacket(const std::vector<char>& buf);
  std::vector<char> key;
  std::vector<char> val;
  uint32_t bod_len;
  uint32_t opaque;
  uint16_t key_len;
  uint16_t reserved;
  char magic;
  char op;
  int ext_len;
  char dat_typ;
  static const int HEADER_LENGTH = 24;
  static const char MAGIC = 0x80;

private:
  uint32_t readUInt32LE(char data[32], size_t offset);
  uint16_t readUInt16LE(char data[32], size_t offset);
  void writeUInt32LE(unsigned char* const data, uint32_t val);
};
