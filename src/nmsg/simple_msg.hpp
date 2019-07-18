#pragma once

#include "../common.hpp"
#include <string>

const uint32_t MAX_OFFSET_NUM = 16;
const uint32_t MAX_HEADER_NUM = 16;
const uint32_t MAX_TAG_NUM = 16;
const uint32_t MAX_ITEM_NUM = 16;

/**
 *  class interface for simple-block-based rdma protocol
 */
namespace r2 {

class Buffer {
public:
  uint64_t offset;
  uint64_t head_ptr;
  uint64_t tag;
  uint64_t size;
}
/**
 * RingBuffer in receiver
 */
class RingBuffer {
public:
  // uint64_t offset[MAX_OFFSET_NUM];
  // uint64_t header_ptrs[MAX_HEADER_NUM];
  // uint64_t tags[MAX_TAG_NUM];
  Buffer buf[MAX_ITEM_NUM];
  uint64_t count = 0;
}

class Session {
public:
  RingBuffer rb;
  std::string remote_addr;

  Session() :
}

} // namespace r2