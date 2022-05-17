//
// Created by crepusculumx on 22-5-10.
//

#include "test.h"

#include "rxcpp-lite/observable.hpp"

/**
 * 测试通过source函数构造的Observable
 * 测试能否正确订阅Next、complete通知
 * 测试是否无视complete之后发出的值
 */
TEST(ObservalbeTest, Int) {
  auto observable = rxcpp_lite::Observable<int>(
      [](rxcpp_lite::Subscriber<int> subscriber) {
        subscriber.next(1);
        subscriber.next(2);
        subscriber.complete();
        subscriber.next(3);// should not get 3
        return []() {};
      }
  );

  auto res = std::vector<int>();
  auto complete_flag = false;

  observable.subscribe(
      rxcpp_lite::Observer<int>(
          [&res](int n) { res.emplace_back(n); },
          [&complete_flag]() { complete_flag = true; },
          [](std::exception &) { FAIL(); }
      )
  );

  EXPECT_EQ(res, std::vector<int>({1, 2}));// should not get 3
  EXPECT_TRUE(complete_flag);
}

/**
 * 测试Observable是单播的
 */
TEST(ObservalbeTest, Unicast) {
  auto observable =
      rxcpp_lite::Observable<std::unique_ptr<int>>(
          [](rxcpp_lite::Subscriber<std::unique_ptr<int>> subscriber) {
            std::unique_ptr<int> res = std::make_unique<int>(1);
            subscriber.next(std::move(res));
            subscriber.complete();
            return []() {};
          }
      );

  std::unique_ptr<int> res1;
  std::unique_ptr<int> res2;

  observable.subscribe(
      rxcpp_lite::Observer<std::unique_ptr<int>>(
          [&res1](std::unique_ptr<int> res) { res1 = std::move(res); },
          []() {},
          [](std::exception &) { FAIL(); }
      )
  );

  observable.subscribe(
      rxcpp_lite::Observer<std::unique_ptr<int>>(
          [&res2](std::unique_ptr<int> res) { res2 = std::move(res); },
          []() {},
          [](std::exception &) { FAIL(); }
      )
  );

  EXPECT_NE(res1, res2);
}