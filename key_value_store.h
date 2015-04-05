#pragma once
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <boost/functional/hash.hpp>

namespace std {
  template <typename T, typename A>
    struct hash<std::vector<T, A>> {
      size_t operator() (const std::vector<T, A>& v) const {
        return boost::hash_range(v.cbegin(), v.cend());
      }
    };
}

class KeyValueStore {
public:
  KeyValueStore () : mut() {}
  void set (const std::vector<char>& key, const std::vector<char>& value);
  const std::vector<char>& get (const std::vector<char>& key);
  bool has (const std::vector<char>& key);
private:
  std::mutex mut;
  std::unordered_map<std::vector<char>, std::vector<char>, std::hash<std::vector<char>>> data;
};

