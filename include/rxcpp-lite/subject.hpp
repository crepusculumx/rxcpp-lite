//
// Created by crepusculumx on 22-5-10.
//

#ifndef RXCPP_LITE_SOURCES_SUBJECT_HPP_
#define RXCPP_LITE_SOURCES_SUBJECT_HPP_

#include <memory>
#include <set>
#include "observable.hpp"
#include "subscriber.hpp"
#include "observer.hpp"

namespace rxcpp_lite {

template<typename T>
class Subject {
 private:
  std::shared_ptr<std::set<std::unique_ptr<Subscriber<T>>>>
      subscribers_ptr_{std::make_shared<std::set<std::unique_ptr<Subscriber<T>>>>()};

  Observable<T> observable_{
      [subscribers_ptr = subscribers_ptr_](rxcpp_lite::Subscriber<T> subscriber) {
        auto &subscribers = *subscribers_ptr;
        auto
            [it, ok] = subscribers.template emplace(std::make_unique<rxcpp_lite::Subscriber<T>>(std::move(subscriber)));

        return [subscribers_ptr = std::move(subscribers_ptr), subscriber_it = it, emplace_ok = ok]() {
          if (!emplace_ok) return;
          auto &subscribers = *subscribers_ptr;
          subscribers.erase(subscriber_it);
        };
      }
  };

  Observer<T> observer_{
      [subscribers_ptr = subscribers_ptr_](auto &&t) {
        auto &subscribers = *subscribers_ptr;
        for (auto it = subscribers.begin(); it != subscribers.end(); it++) {
          it->get()->template next(t);
        }
      },
      [subscribers_ptr = subscribers_ptr_]() {
        auto &subscribers = *subscribers_ptr;
        for (auto it = subscribers.begin(); it != subscribers.end(); it++) {
          it->get()->complete();
        }
      },
      [subscribers_ptr_ = subscribers_ptr_](std::exception &e) {
        auto &subscribers = *subscribers_ptr_;
        for (auto it = subscribers.begin(); it != subscribers.end(); it++) {
          it->get()->error(e);
        }
      },
  };
 public:
  Subject() = default;

  Subscription subscribe(Observer<T> observer) {
    return observable_.subscribe(std::move(observer));
  }

  template<typename V>
  void next(V &&v) {
    observer_.next(std::forward<V>(v));
  }

  void complete() {
    observer_.complete();
  }

  void error(std::exception &e) {
    observer_.error(e);
  }
};

}

#endif //RXCPP_LITE_SOURCES_SUBJECT_HPP_
