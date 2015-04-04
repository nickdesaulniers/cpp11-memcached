#include <iostream>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Packet {
public:
  // 32 bytes for a get request
  Packet (char data [24]): bod_len(readUInt32LE(data, 8)), opaque(readUInt32LE(data, 12)), key_len(readUInt16LE(data, 2)), op(data[1]), ext_len(data[4]) {}
  void read (tcp::socket& socket);
  void write (tcp::socket& socket);
  void respondToGet (tcp::socket& socket, const std::vector<char>& val);
  void set (const std::vector<char>& key, const std::vector<char>& val);
  static void printPacket (const char* const buf, const size_t len);
  static void printPacket (const std::vector<char>& buf);
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
private:
  uint32_t readUInt32LE (char data [32], size_t offset);
  uint16_t readUInt16LE (char data [32], size_t offset);
  void writeUInt32LE (char* const data, uint32_t val);
};
