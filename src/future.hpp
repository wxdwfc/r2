#pragma once

namespace r2 {

template <typename T>
class Future {
 public:
  virtual T poll() = 0;
};

}
