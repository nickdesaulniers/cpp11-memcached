#cpp11-memcached
This is a toy memcached server that simply supports set and get requests, written as an opportunity to write some more c++11.

##Building
Requirements:

* C++11 compatible compiler
* CMake
* Boost
* Node.js (optional, for running some of the local tests)

```
mkdir build && cd buid
cmake ..
make
./main
```

For Boost & Cmake, you can install it on OSX with `brew install boost cmake` and on Ubuntu with `sudo aptitude install cmake libboost-all-dev`.

###CAS Field
The CAS field is used to signal atomicity.  From the [memcached spec](https://code.google.com/p/memcached/wiki/MemcacheBinaryProtocol), section 4.3 "Set, Add, Replace":

```
If the Data Version Check (CAS) is nonzero, the requested operation
MUST only succeed if the item exists and has a CAS value identical to
the provided value.
```

CAS is short for [Compare And Swap](http://en.wikipedia.org/wiki/Compare-and-swap), a common atomic operation that only updates a value if the writer has the most up to date value.  For example, if thread A writes to a location, then thread B writes to the same location, thread A's subsequent CAS operation will fail.

###Tradeoffs
1. This code assumes all key value pairs can fit into memory on this machine.  This doesn't scale well beyond a single machine.  Moving to a distributed message queue might be better, but adds [complexity in keeping all caches coherent](http://martinfowler.com/bliki/TwoHardThings.html).
2. Endianess.  The code that handles converting from bytes to ints depends on the host being little endian (like x86-64, ARM devices might have trouble).  This part of the code should be rewritten to be more portable.
3. One thread is spawned per connection.  This adds latency as the thread is being spawned, and can easily use up system resources in a DoS attack.  Instead, a thread pool should be used.
4. Connections are handled synchronously.  This hurts throughput, but makes the code slightly simpler.  Boost asio has support for async.
5. All threads share a key value store.  Shared memory is juggling razors.  Also, I simply wrap access to the key value store in a mutex.  A thread must acquire the mutex once for a set operation and twice for a get operation.  The use of a properly programmed concurrent map would be better, like from [libcds](https://github.com/khizmax/libcds) or even the basic example from [C++ Concurrency in Action: Practical Multithreading](http://www.cplusplusconcurrencyinaction.com/).

###Formatting
Each C++ source and header is run through `clang-format -style=Mozilla -i <file>`.

