#include "key_value_store.h"

void KeyValueStore::set(const std::vector<char>& key, const FlaggedValue& fv) {
  std::lock_guard<std::mutex> lk(mut);
  try {
    data[key] = fv;
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

std::pair<bool, const FlaggedValue&> KeyValueStore::get(
    const std::vector<char>& key) {
  std::lock_guard<std::mutex> lk(mut);
  return std::make_pair(data.count(key) > 0, std::ref(data[key]));
}
