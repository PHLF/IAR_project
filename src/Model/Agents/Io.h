#ifndef IO_H
#define IO_H

#include <functional>

namespace sim {

struct IO {
  std::function<bool(void)> input;
  std::function<void(void)> output;
};

}  // namespace sim

#endif  // IO_H
