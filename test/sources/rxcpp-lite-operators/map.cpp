//
// Created by crepusculumx on 22-5-17.
//



#include "test.h"

#include "rxcpp-lite/observable.hpp"
#include "rxcpp-lite/rxcpp-lite-operators/map.hpp"

/**
 * 测试使用map操作符进行类型转换
 */
TEST(MapOperatorTest, IntToChar) {
  auto observable = rxcpp_lite::Observable<int>(
      [](rxcpp_lite::Subscriber<int> subscriber) {
        subscriber.next(1);
        subscriber.next(2);
        subscriber.complete();
        return []() {};
      }
  );

  auto res = std::vector<char>();
  auto complete_flag = false;
  observable.pipe(rxcpp_lite::map<int, char>([](int n) { return char(n + '0'); })).subscribe(
      rxcpp_lite::Observer<char>(
          [&res](char n) { res.emplace_back(n); },
          [&complete_flag]() { complete_flag = true; },
          [](std::exception &) { FAIL(); }
      )
  );

  EXPECT_EQ(res, std::vector<char>({'1', '2'}));// should not get 3
  EXPECT_TRUE(complete_flag);

}

/**
 * 测试多个map
 */
TEST(MapOperatorTest, MultiMap) {
  auto m = rxcpp_lite::map<int, double>{[](int n) { return double(n); }};
  auto observable = rxcpp_lite::Observable<int>(
      [](rxcpp_lite::Subscriber<int> subscriber) {
        subscriber.next(1);
        subscriber.next(2);
        subscriber.complete();
        return []() {};
      }
  );

  auto res = std::vector<char>();
  auto complete_flag = false;
  observable.pipe(
      rxcpp_lite::map<int, int>([](int n) { return n + 1; }),
      rxcpp_lite::map<int, int>([](int n) { return n + 1; }),
      rxcpp_lite::map<int, char>([](int n) { return char(n + '0'); })
  ).subscribe(
      rxcpp_lite::Observer<char>(
          [&res](char n) { res.emplace_back(n); },
          [&complete_flag]() { complete_flag = true; },
          [](std::exception &) { FAIL(); }
      )
  );

  EXPECT_EQ(res, std::vector<char>({'3', '4'}));// should not get 3
  EXPECT_TRUE(complete_flag);

}