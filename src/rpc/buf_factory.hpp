#pragma once

#include "../common.hpp"
#include "../allocator_master.hpp"

namespace r2 {

namespace rpc {

const int MAX_INLINE_SIZE = 64;
const int MAX_ROLLING_IDX = 64;

class BufFactory {
  struct rpc_buf_t {
    char data[MAX_INLINE_SIZE];
  };

 public:
  explicit BufFactory(int padding) : extra_padding(padding) {
  }

  char *alloc(int size) const {
    //char *ptr = (char *)Rmalloc(size + extra_padding);
    char *ptr = (char *)(AllocatorMaster<>::get_thread_allocator()->alloc(size + extra_padding));
    //LOG(4) << "alloc ptr: " << (void *)ptr << " for sz: " << size;
    if(likely(ptr != nullptr))
      return ptr + extra_padding;
    return nullptr;
  }

  void  dealloc(char *ptr) const {
    (AllocatorMaster<>::get_thread_allocator()->free(ptr - extra_padding));
  }

  char *get_inline_buf(int idx = 0) {
    return &inline_bufs[idx * MAX_INLINE_SIZE] + extra_padding;
  }

  char *get_inline() {
    rolling_idx_ = (rolling_idx_ + 1) % MAX_ROLLING_IDX;
    return get_inline_buf(rolling_idx_);
  }

 private:
  const int extra_padding = 0; // extra padding used for each message
  char      inline_bufs[MAX_INLINE_SIZE * MAX_ROLLING_IDX];
  u16       rolling_idx_ = 0;
}; // end class

} // end namespace rpc

} // end namespace r2
