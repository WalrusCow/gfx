#include <memory>
#include <utility>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  // Pining for C++14...
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
