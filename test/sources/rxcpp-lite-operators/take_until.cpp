//
// Created by crepusculumx on 22-5-24.
//


#include <rxcpp-lite/async-subject.hpp>
#include "test.h"

#include "rxcpp-lite/observable.hpp"
#include "rxcpp-lite/rxcpp-lite-operators/take_until.hpp"


/**
 * 测试通过takeUntil操作符取消订阅
 */
TEST(TakeUntilTest, Int) {
  auto unsubscribe_flag_ptr = std::make_shared<bool>(false);
  auto observable = rxcpp_lite::Observable<int>(
      [unsubscribe_flag_ptr](rxcpp_lite::Subscriber<int> subscriber) {
        subscriber.next(1);
        subscriber.next(2);
        subscriber.complete();
        return [unsubscribe_flag_ptr]() { *unsubscribe_flag_ptr = true; };
      }
  );

  auto res = std::vector<int>();
  auto complete_flag = false;

  auto subject = rxcpp_lite::AsyncSubject<bool>();

  observable.pipe(
      rxcpp_lite::take_until<int, bool>(subject.asObservable())
  ).subscribe(
      rxcpp_lite::Observer<int>(
          [&res](int n) { res.emplace_back(n); },
          [&complete_flag]() { complete_flag = true; },
          [](std::exception &) { FAIL(); }
      )
  );

  EXPECT_EQ(res, std::vector<int>({1, 2}));
  EXPECT_TRUE(complete_flag);
}