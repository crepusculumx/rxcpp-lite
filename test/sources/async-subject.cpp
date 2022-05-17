//
// Created by crepusculumx on 22-5-17.
//

#include "test.h"

#include "rxcpp-lite/async-subject.hpp"

/**
 * 测试Subject的基本功能
 * 测试能否正确订阅Next、complete通知
 * 测试是否无视complete之后发出的值
 * 测试默认BUFFER_SIZE = 1
 */
TEST(AsyncSubjectTest, OneInt) {
  auto subject = rxcpp_lite::AsyncSubject<int>();

  auto res = std::vector<int>();
  auto complete_flag = false;

  subject.next(1);

  subject.subscribe(
      rxcpp_lite::Observer<int>(
          [&res](int n) { res.emplace_back(n); },
          [&complete_flag]() { complete_flag = true; },
          [](std::exception &) { FAIL(); }
      )
  );

  // should not get 1
  EXPECT_EQ(res, std::vector<int>({}));
  EXPECT_FALSE(complete_flag);

  subject.next(2);
  subject.complete();
  EXPECT_EQ(res, std::vector<int>({2})); // should not get 1
  EXPECT_TRUE(complete_flag);

}

/**
 * 测试Subject的基本功能
 * 测试能否正确订阅Next、complete通知
 * 测试是否无视complete之后发出的值
 * 测试BUFFER_SIZE = 2
 */
TEST(AsyncSubjectTest, TestBufferSize) {
  auto subject = rxcpp_lite::AsyncSubject<int, 2>();

  auto res = std::vector<int>();
  auto complete_flag = false;

  subject.next(1);
  subject.next(2);
  subject.next(3);
  subject.next(4);
  subject.next(5);

  subject.subscribe(
      rxcpp_lite::Observer<int>(
          [&res](int n) { res.emplace_back(n); },
          [&complete_flag]() { complete_flag = true; },
          [](std::exception &) { FAIL(); }
      )
  );

  // should not get any
  EXPECT_EQ(res, std::vector<int>({}));
  EXPECT_FALSE(complete_flag);

  subject.complete();
  EXPECT_EQ(res, std::vector<int>({4, 5})); // should not get 1
  EXPECT_TRUE(complete_flag);

}

/**
 * 测试AsyncSubject是多播的
 */
TEST(AsyncSubjectTest, Multicast) {
  auto subject = rxcpp_lite::AsyncSubject<std::shared_ptr<int>>();

  std::shared_ptr<int> res1;
  std::shared_ptr<int> res2;

  subject.subscribe(
      rxcpp_lite::Observer<std::shared_ptr<int>>(
          [&res1](std::shared_ptr<int> res) { res1 = std::move(res); },
          []() {},
          [](std::exception &) { FAIL(); }
      ));

  subject.subscribe(
      rxcpp_lite::Observer<std::shared_ptr<int>>(
          [&res2](std::shared_ptr<int> res) { res2 = std::move(res); },
          []() {},
          [](std::exception &) { FAIL(); }
      ));

  subject.next(std::make_shared<int>(1));
  subject.complete();
  EXPECT_EQ(res1, res2);
}