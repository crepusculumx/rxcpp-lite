//
// Created by crepusculumx on 22-5-17.
//


#include "test.h"

#include "rxcpp-lite/observable.hpp"

/**
 * 测试Observer完整的构造函数
 */
TEST(ObservalbeTest, FullConstructor) {
  auto observable = rxcpp_lite::Observable<int>(
      [](rxcpp_lite::Subscriber<int> subscriber) {
        subscriber.next(1);
        subscriber.next(2);
        subscriber.complete();
        return []() {};
      }
  );

  auto res = std::vector<int>();
  auto complete_flag = false;

  auto observer = rxcpp_lite::Observer<int>(
      [&res](int n) { res.emplace_back(n); },
      [&complete_flag]() { complete_flag = true; },
      [](std::exception &) { FAIL(); });

  observable.subscribe(observer);

  EXPECT_EQ(res, std::vector<int>({1, 2}));
  EXPECT_TRUE(complete_flag);
}

/**
 * 测试Observer简单的仅含next的构造函数
 */
TEST(ObservalbeTest, liteConstructor) {
  auto observable = rxcpp_lite::Observable<int>(
      [](rxcpp_lite::Subscriber<int> subscriber) {
        subscriber.next(1);
        subscriber.next(2);
        subscriber.complete();
        return []() {};
      }
  );

  auto res = std::vector<int>();

  auto observer = rxcpp_lite::Observer<int>([&res](int n) { res.emplace_back(n); });

  observable.subscribe(observer);

  EXPECT_EQ(res, std::vector<int>({1, 2}));
}