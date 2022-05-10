//
// Created by crepusculumx on 2022/4/13.
//

#ifndef RXCPPLITE_SRC_SUBSCRIBER_HPP_
#define RXCPPLITE_SRC_SUBSCRIBER_HPP_

#include <exception>

#include "observer.hpp"

namespace rxcpp_lite {

template<typename T>
class Subscriber {
 private:
  Observer<T> observer;
  bool is_stop = false;

 public:
  Subscriber() = default;

  Subscriber(const Subscriber<T> &subscriber) : observer(subscriber.observer), is_stop(subscriber.is_stop) {}

  Subscriber(Subscriber<T> &&subscriber) noexcept
      : observer(std::move(subscriber.observer)), is_stop(std::move(subscriber.is_stop)) {}

  explicit Subscriber(const Observer<T> &observer) : observer(observer) {}

  template<typename V>
  void next(V &&v) {
    if (is_stop) return;
    observer.next(std::forward<V>(v));
  }

  void complete() {
    is_stop = true;
    observer.complete();
  }

  void error(std::exception &e) {
    is_stop = true;
    observer.error(e);
  }
};

}
#endif //RXCPPLITE_SRC_SUBSCRIBER_HPP_