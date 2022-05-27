//
// Created by crepusculumx on 22-5-14.
//

#ifndef RXCPPLITETEST_INCLUDE_RXCPP_LITE_ASYNC_SUBJECT_HPP_
#define RXCPPLITETEST_INCLUDE_RXCPP_LITE_ASYNC_SUBJECT_HPP_

#include "rxcpp-lite-include.hpp"

#include "subject.hpp"

namespace rxcpp_lite {

namespace detail {

template<typename T>
class AsyncSubjectBase {
 protected:
  std::shared_ptr<std::list<T>> values_ptr{std::make_shared<std::list<T>>()};
  std::shared_ptr<bool> is_complete_ptr{std::make_shared<bool>(false)};

 public:
  AsyncSubjectBase() = default;
  AsyncSubjectBase(const AsyncSubjectBase &) = default;
  AsyncSubjectBase(AsyncSubjectBase &&) noexcept = default;
  AsyncSubjectBase &operator=(const AsyncSubjectBase &) = default;
  AsyncSubjectBase &operator=(AsyncSubjectBase &&) noexcept = default;
};

}

template<typename T, size_t BUFFER_SIZE = 1>
class AsyncSubject :
    public detail::AsyncSubjectBase<T>,
    public detail::SubjectBase<T>,
    public Observable<T>,
    public Observer<T> {

 public:
  AsyncSubject() :
      detail::AsyncSubjectBase<T>(),
      Observable<T>(
          [
              subscribers_ptr = this->subscribers_ptr,
              values_ptr = this->values_ptr,
              is_complete_ptr = this->is_complete_ptr
          ](rxcpp_lite::Subscriber<T> subscriber) {
            auto &subscribers = *subscribers_ptr;
            auto [subscriber_it, ok] =
                subscribers.template emplace(std::make_unique<rxcpp_lite::Subscriber<T>>(std::move(subscriber)));

            // 对已经complete的Subject的订阅，依然能受到值（如果有的话）,直接发送值。
            // buffer_size由Observer保证，保证next发送的数量不超过buffer_size
            auto is_complete = *is_complete_ptr;
            if (is_complete) {
              auto &values = *values_ptr;
              for (auto it = values.cbegin(); it != values.cend(); it++) {
                subscriber.template next(*it);
              }
              subscriber.complete();
            }

            return [
                subscribers_ptr = std::move(subscribers_ptr),
                subscriber_it = std::move(subscriber_it),
                emplace_ok = ok
            ]() {
              if (!emplace_ok) return;
              auto &subscribers = *subscribers_ptr;
              subscribers.erase(subscriber_it);
            };
          }
      ),
      Observer<T>(
          [
              values_ptr = this->values_ptr,
              buffer_size = BUFFER_SIZE,
              is_complete_ptr = this->is_complete_ptr
          ](T t) {
            auto &values = *values_ptr;
            if (values.size() == buffer_size) values.pop_front();
            values.template emplace_back(std::move(t));
          },
          [
              subscribers_ptr = this->subscribers_ptr,
              values_ptr = this->values_ptr,
              is_complete_ptr = this->is_complete_ptr
          ]() {
            auto &subscribers = *subscribers_ptr;
            auto &values = *values_ptr;
            *is_complete_ptr = true;
            for (auto subscriber_it = subscribers.begin(); subscriber_it != subscribers.end(); subscriber_it++) {
              // 当complete时，发送值
              for (auto value_it = values.cbegin(); value_it != values.cend(); value_it++) {
                subscriber_it->get()->template next(*value_it);
              }
              subscriber_it->get()->complete();
            }
          },
          [subscribers_ptr_ = this->subscribers_ptr](std::exception &e) {
            auto &subscribers = *subscribers_ptr_;
            for (auto it = subscribers.begin(); it != subscribers.end(); it++) {
              it->get()->error(e);
            }
          }
      ) {}

  Observable<T> asObservable() {
    return Observable<T>(*this);
  }
};
}

#endif //RXCPPLITETEST_INCLUDE_RXCPP_LITE_ASYNC_SUBJECT_HPP_
