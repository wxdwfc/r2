#pragma once

namespace r2 {

template <typename T>
class Future {
 public:
  explicit Future(int cid) : cor_id(cid) {
  }

  virtual int poll() = 0;
  const   int cor_id;
  T       res;
};

}
