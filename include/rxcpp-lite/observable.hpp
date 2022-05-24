#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#ifndef RXCPPLITE_SRC_OBSERVABLE_HPP_
#define RXCPPLITE_SRC_OBSERVABLE_HPP_

#include "rxcpp-lite-include.hpp"

#include "observer.hpp"
#include "subscriber.hpp"
#include "subscription.hpp"

namespace rxcpp_lite {

template<typename T>
class Observable {
 private:
  std::function<std::function<void(void)>(Subscriber<T>)> source;

  template<typename V, typename RxOperator>
  auto pipe_from(Observable<V> observable, RxOperator rx_operator) {
    return rx_operator(observable);
  }

  template<typename V, typename RxOperator, typename ...RxOperators>
  auto pipe_from(Observable<V> observable, RxOperator rx_operator, RxOperators ...rx_operators) {
    auto res = rx_operator(std::move(observable));
    return pipe_from(std::move(res), rx_operators...);
  }

 public:
  Observable() : source([](Subscriber<T>) { return []() {}; }) {}

  Observable &operator=(const Observable observable) {
    source = std::move(observable.source);
    return *this;
  }

  Observable(const Observable<T> &observable)
      : source(observable.source) {}

  Observable(Observable<T> &&observable)
  noexcept: source(std::move(observable.source)) {}

  explicit Observable(std::function<std::function<void(void)>(Subscriber<T>)> source) : source(std::move(source)) {}

  Subscription subscribe(Observer<T> observer) {
    auto subscriber = Subscriber<T>(std::move(observer));
    auto on_unsubscribe = source(std::move(subscriber));
    return Subscription(std::move(on_unsubscribe));
  }

  template<typename ...RxOperators>
  auto pipe(RxOperators... rx_operators) {
    return pipe_from(*this, rx_operators...);
  }
};
}
#endif //RXCPPLITE_SRC_OBSERVABLE_HPP_

#pragma clang diagnostic pop