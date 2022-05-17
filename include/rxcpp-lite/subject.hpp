//
// Created by crepusculumx on 22-5-10.
//

#ifndef RXCPP_LITE_SOURCES_SUBJECT_HPP_
#define RXCPP_LITE_SOURCES_SUBJECT_HPP_

#include "rxcpp-lite-include.hpp"

#include "observable.hpp"
#include "subscriber.hpp"
#include "observer.hpp"

namespace rxcpp_lite {

namespace detail {

template<typename T>
class SubjectBase {
 protected:
  std::shared_ptr<std::set<std::unique_ptr<Subscriber<T>>>>
      subscribers_ptr_{std::make_shared<std::set<std::unique_ptr<Subscriber<T>>>>()};

 public:
  SubjectBase() = default;
  SubjectBase(const SubjectBase &) = delete;
  SubjectBase(SubjectBase &&) = delete;
  SubjectBase &operator=(const SubjectBase &) = delete;
  SubjectBase &operator=(SubjectBase &&) = delete;
};
}

template<typename T>
class Subject :
    public detail::SubjectBase<T>,
    public Observable<T>,
    public Observer<T> {
 public:
  Subject() :
      detail::SubjectBase<T>(),
      Observable<T>(
          [subscribers_ptr = this->subscribers_ptr_](rxcpp_lite::Subscriber<T> subscriber) {
            auto &subscribers = *subscribers_ptr;
            auto [it, ok] =
                subscribers.template emplace(std::make_unique<rxcpp_lite::Subscriber<T>>(std::move(subscriber)));

            return [subscribers_ptr = std::move(subscribers_ptr), subscriber_it = it, emplace_ok = ok]() {
              if (!emplace_ok) return;
              auto &subscribers = *subscribers_ptr;
              subscribers.erase(subscriber_it);
            };
          }
      ),
      Observer<T>(
          [subscribers_ptr = this->subscribers_ptr_](T t) {
            auto &subscribers = *subscribers_ptr;
            for (auto it = subscribers.begin(); it != subscribers.end(); it++) {
              it->get()->template next(t);
            }
          },
          [subscribers_ptr = this->subscribers_ptr_]() {
            auto &subscribers = *subscribers_ptr;
            for (auto it = subscribers.begin(); it != subscribers.end(); it++) {
              it->get()->complete();
            }
          },
          [subscribers_ptr_ = this->subscribers_ptr_](std::exception &e) {
            auto &subscribers = *subscribers_ptr_;
            for (auto it = subscribers.begin(); it != subscribers.end(); it++) {
              it->get()->error(e);
            }
          }
      ) {}

};

}

#endif //RXCPP_LITE_SOURCES_SUBJECT_HPP_
