//
// Created by crepusculumx on 2022/4/13.
//

#ifndef RXCPPLITE_SRC_OBSERVER_HPP_
#define RXCPPLITE_SRC_OBSERVER_HPP_

#include <functional>

namespace rxcpp_lite {

template<typename T>
class Observer {
 private:
  std::function<void(T)> on_next;
  std::function<void(void)> on_complete;
  std::function<void(std::exception &)> on_error;

 public:
  Observer() : on_next([](T) {}), on_complete([]() {}), on_error([](std::exception &) {}) {};

  Observer &operator=(const Observer observer) {
    on_next = std::move(observer.on_next);
    on_complete = std::move(observer.on_complete);
    on_error = std::move(observer.on_error);
    return *this;
  }

  Observer(const Observer<T> &observer)
      : on_next(observer.on_next), on_complete(observer.on_complete), on_error(observer.on_error) {}

  Observer(Observer<T> &&observer)
  noexcept: on_next(std::move(observer.on_next)),
            on_complete(std::move(observer.on_complete)),
            on_error(std::move(observer.on_error)) {}

  Observer(std::function<void(T)> on_next,
           std::function<void(void)> on_complete,
           std::function<void(std::exception &)> on_error)
      : on_next(std::move(on_next)), on_complete(std::move(on_complete)), on_error(std::move(on_error)) {}

  explicit Observer(std::function<void(T)> on_next)
      : on_next(std::move(on_next)), on_complete([]() {}), on_error([](const std::exception &) {}) {}

  template<typename V>
  void next(V &&v) {
    on_next(std::forward<V>(v));
  }

  void complete() {
    on_complete();
  }

  void error(std::exception &e) {
    on_error(e);
  }
};

}
#endif //RXCPPLITE_SRC_OBSERVER_HPP_
