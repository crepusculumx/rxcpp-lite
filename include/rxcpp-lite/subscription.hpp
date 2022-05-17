//
// Created by crepusculumx on 2022/4/13.
//

#ifndef RXCPPLITE_SRC_SUBSCRIPTION_HPP_
#define RXCPPLITE_SRC_SUBSCRIPTION_HPP_

#include "rxcpp-lite-include.hpp"

namespace rxcpp_lite {

class Subscription {
 private:
  std::function<void(void)> on_unsubscribe;
 public:
  Subscription() : on_unsubscribe([]() {}) {}

  explicit Subscription(std::function<void(void)> on_unsubscribe) : on_unsubscribe(std::move(on_unsubscribe)) {}

  Subscription(const Subscription &subscription) = default;

  Subscription(Subscription &&subscription) noexcept: on_unsubscribe(std::move(subscription.on_unsubscribe)) {}

  void unsubscribe() {
    on_unsubscribe();
  }
};

}

#endif //RXCPPLITE_SRC_SUBSCRIPTION_HPP_
