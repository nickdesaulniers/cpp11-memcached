#include "key_value_store.h"

void KeyValueStore::set(const std::vector<char>& key, const FlaggedValue& fv) {
  std::lock_guard<std::mutex> lk(mut);
  try {
    data[key] = fv;
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

const FlaggedValue& KeyValueStore::get(const std::vector<char>& key) {
  std::lock_guard<std::mutex> lk(mut);
  return data[key];
}

bool KeyValueStore::has(const std::vector<char>& key) {
  std::lock_guard<std::mutex> lk(mut);
  return data.count(key) > 0;
}
