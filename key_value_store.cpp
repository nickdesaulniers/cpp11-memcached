#include "key_value_store.h"

void KeyValueStore::set (const std::vector<char>& key, const std::vector<char>& value) {
  //std::cout << "set kvs, size: " << data.size() << std::endl;
  std::lock_guard<std::mutex> lk (mut);
  //std::cout << "lock" << std::endl;
  try {
    data[key] = value;
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  //std::cout << "set kvs after, size: " << data.size() << std::endl;
}

const std::vector<char>& KeyValueStore::get (const std::vector<char>& key) {
  //std::cout << "get kvs, size: " << data.size() << std::endl;
  std::lock_guard<std::mutex> lk (mut);
  //std::cout << "lock" << std::endl;
  return data[key];
}

bool KeyValueStore::has (const std::vector<char>& key) {
  std::lock_guard<std::mutex> lk (mut);
  return data.count(key) > 0;
}

