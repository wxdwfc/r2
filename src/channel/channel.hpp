#pragma once

namespace r2 {


template <class T>
class Channel {
  static const uint64_t ENTRY_SIZE = CACHE_LINE_SZ;

 private:
  struct Entry {
    T value;
  } __attribute__((aligned(ENTRY_SIZE)));

 public:
  volatile uint64_t __attribute__((aligned(CACHE_LINE_SZ))) head;
  volatile uint64_t __attribute__((aligned(CACHE_LINE_SZ))) tail;
  Entry *ring_buf;
  uint64_t max_entry_num;

 public:
  Channel(uint64_t max_entry_num = 1)
      : max_entry_num(max_entry_num), head(0), tail(0) {
    assert(!(max_entry_num & (max_entry_num - 1)));

    ring_buf = static_cast<Entry *>(
        std::aligned_alloc(CACHE_LINE_SZ, max_entry_num * sizeof(Entry)));
    assert(ring_buf != nullptr);
    assert((uint64_t)ring_buf % ENTRY_SIZE == 0);
  }

  ~Channel() { free(ring_buf); }

  inline uint64_t size() { return head - tail; }

  inline bool isEmpty() { return head == tail; }

  inline bool enqueue(const T &value) {
    assert(head >= tail);
    if (head == tail + max_entry_num) {
      return false;
    } else {
      uint64_t index = head & (max_entry_num - 1);

      ring_buf[index].value = value;
      __sync_fetch_and_add(&head, 1);
      return true;
    }
  }

  inline void enqueue_blocking(const T &value) {
    assert(head >= tail);
    while (head == tail + max_entry_num)
      ;
    uint64_t index = head & (max_entry_num - 1);

    ring_buf[index].value = value;
    __sync_fetch_and_add(&head, 1);
  }

  inline T dequeue_blocking() {
    assert(head >= tail);
    while (head == tail)
      ;
    uint64_t index = tail & (max_entry_num - 1);

    T ret = ring_buf[index].value;
    __sync_fetch_and_add(&tail, 1);
    return ret;
  }

  inline bool dequeue(T &value) {
    assert(head >= tail);
    if (head == tail) {
      return false;
    } else {
      uint64_t index = tail & (max_entry_num - 1);

      value = ring_buf[index].value;
      __sync_fetch_and_add(&tail, 1);
      return true;
    }
  }
} __attribute__((aligned(CACHE_LINE_SZ)));


} // end namespace r2
