#pragma once

#include "../common.hpp"
#include "../allocator_master.hpp"

namespace r2 {

namespace rpc {

const int MAX_INLINE_SIZE = 64;

class BufFactory {
 public:
  explicit BufFactory(int padding) : extra_padding(padding) {
  }

  char *alloc(int size) const {
    //char *ptr = (char *)Rmalloc(size + extra_padding);
    char *ptr = (char *)(AllocatorMaster<>::get_thread_allocator()->alloc(size + extra_padding));
    if(likely(ptr != nullptr))
      return ptr + extra_padding;
    return nullptr;
  }

  void  dealloc(char *ptr) const {
    (AllocatorMaster<>::get_thread_allocator()->free(ptr));
  }

  char *get_inline_buf() {
    return inline_buf + extra_padding;
  }
 private:
  const int extra_padding = 0; // extra padding used for each message
  char      inline_buf[MAX_INLINE_SIZE];
}; // end class

} // end namespace rpc

} // end namespace r2
