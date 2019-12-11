#pragma once

#include "../mem_block.hh"

namespace r2 {

namespace ring_msg {

struct Ring {
  MemBlock local_mem;
  usize tailer = 0;
  usize header = 0;

  explicit Ring(const MemBlock &mem) : local_mem(mem) {}

  Ring() : Ring(MemBlock(nullptr, 0)) {}
  ~Ring() = default;

  inline Option<MemBlock> cur_msg(const usize &sz) {
    if (unlikely(sz + tailer > local_mem.sz)) {
      return {};
    }
    auto temp = increment_tailer(sz);
    return {.mem_ptr = (char *)(local_mem.mem_ptr + temp), .sz = sz};
  }

private:
  // wo check, so it's private
  usize increment_tailer(const usize &sz) {
    auto cur_tailer = tailer;
    tailer = (tailer + sz) % local_mem.sz;
    return cur_tailer;
  }
};
} // namespace ring_msg

} // namespace r2
