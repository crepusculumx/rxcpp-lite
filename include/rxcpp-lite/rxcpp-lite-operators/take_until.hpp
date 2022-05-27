//
// Created by crepusculumx on 22-5-24.
//

#ifndef RXCPPLITETEST_INCLUDE_RXCPP_LITE_RXCPP_LITE_OPERATORS_TAKE_UNTIL_HPP_
#define RXCPPLITETEST_INCLUDE_RXCPP_LITE_RXCPP_LITE_OPERATORS_TAKE_UNTIL_HPP_

#include "rxcpp-lite/rxcpp-lite-include.hpp"
#include "rxcpp-lite/observer.hpp"
#include "rxcpp-lite/observable.hpp"

namespace rxcpp_lite {

template<typename T, typename V>
class take_until {
 private:
  Observable<V> source;
 public:
  explicit take_until(Observable<V> source) : source(std::move(source)) {}

  Observable<T> operator()(Observable<T> observable) {
    // 返回一个新Observable, 类型为转换后的V，对该Observable的订阅转为对源的订阅
    return Observable<T>{
        [
            observable = std::move(observable),
            source = this->source
        ](Subscriber<T> subscriber) mutable {
          auto subscriber_ptr = std::make_shared<Subscriber<T>>(std::move(subscriber));

          auto upper_subscription = observable.subscribe(Observer<T>{
              [subscriber_ptr](T t) { subscriber_ptr->template next(t); },
              [subscriber_ptr]() { subscriber_ptr->complete(); },
              [subscriber_ptr](std::exception &e) { subscriber_ptr->error(e); }
          });

          auto upper_subscription_ptr = std::make_shared<Subscription>(std::move(upper_subscription));

          auto source_subscription = source.subscribe(Observer<V>{
              [upper_subscription_ptr](V v) { upper_subscription_ptr->unsubscribe(); },
              [upper_subscription_ptr]() { upper_subscription_ptr->unsubscribe(); },
              [](std::exception &e) {}
          });

          auto source_subscription_ptr = std::make_shared<Subscription>(std::move(source_subscription));
          return [
              upper_subscription_ptr = std::move(upper_subscription_ptr),
              source_subscription_ptr = std::move(source_subscription_ptr)
          ]() {
            upper_subscription_ptr->unsubscribe();
            source_subscription_ptr->unsubscribe();
          };
        }};
  }
};
}

#endif //RXCPPLITETEST_INCLUDE_RXCPP_LITE_RXCPP_LITE_OPERATORS_TAKE_UNTIL_HPP_
