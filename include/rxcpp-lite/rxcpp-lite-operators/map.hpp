//
// Created by crepusculumx on 22-5-17.
//

#ifndef RXCPPLITETEST_INCLUDE_RXCPP_LITE_RXCPP_LITE_OPERATORS_MAP_HPP_
#define RXCPPLITETEST_INCLUDE_RXCPP_LITE_RXCPP_LITE_OPERATORS_MAP_HPP_

#include "rxcpp-lite/rxcpp-lite-include.hpp"
#include "rxcpp-lite/observer.hpp"
#include "rxcpp-lite/observable.hpp"

namespace rxcpp_lite {

template<typename T, typename V>
class map {
 private:
  std::function<V(T)> function_;
 public:
  explicit map(std::function<V(T)> function) : function_(std::move(function)) {}

  Observable<V> from(Observable<T> observable) {
    // 返回一个新Observable, 类型为转换后的V，对该Observable的订阅转为对源的订阅
    return Observable<V>{
        [observable = std::move(observable),
            map_function = this->function_
        ](Subscriber<V> subscriber) mutable {
          auto subscriber_ptr = std::make_shared<Subscriber<V>>(std::move(subscriber));

          auto subscription = observable.subscribe(Observer<T>{
              [subscriber_ptr, map_function = std::move(map_function)](T t) {
                subscriber_ptr->template next(map_function(t));
              },
              [subscriber_ptr]() { subscriber_ptr->complete(); },
              [subscriber_ptr](std::exception &e) { subscriber_ptr->error(e); }
          });

          return [subscription]() mutable {
            subscription.unsubscribe();
          };
        }};
  }
};
}

#endif //RXCPPLITETEST_INCLUDE_RXCPP_LITE_RXCPP_LITE_OPERATORS_MAP_HPP_
