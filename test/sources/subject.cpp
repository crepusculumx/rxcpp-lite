//
// Created by crepusculumx on 22-5-10.
//

#include <vector>

#include "gtest/gtest.h"
#include "rxcpp-lite/subject.hpp"

/**
 * 测试Subject的基本功能
 * 测试能否正确订阅Next、complete通知
 * 测试是否无视complete之后发出的值
 */
TEST(SubjectTest, Int) {
  auto subject = rxcpp_lite::Subject<int>();

  auto res = std::vector<int>();
  auto complete_flag = false;

  subject.next(1);

  // should not get 1
  subject.subscribe(
      rxcpp_lite::Observer<int>(
          [&res](int n) { res.emplace_back(n); },
          [&complete_flag]() { complete_flag = true; },
          [](std::exception &) { FAIL(); }
      )
  );

  EXPECT_EQ(res, std::vector<int>({}));
  EXPECT_FALSE(complete_flag);

  subject.next(2);
  EXPECT_EQ(res, std::vector<int>({2}));
  EXPECT_FALSE(complete_flag);

  subject.next(3);
  EXPECT_EQ(res, std::vector<int>({2, 3}));
  EXPECT_FALSE(complete_flag);

  subject.complete();
  EXPECT_TRUE(complete_flag);

  subject.next(4); //should not get
  EXPECT_EQ(res, std::vector<int>({2, 3}));
  EXPECT_TRUE(complete_flag);
}

/**
 * 测试Observable是多播的
 */
TEST(SubjectTest, Multicast) {
  auto subject = rxcpp_lite::Subject<std::shared_ptr<int>>();

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
  EXPECT_EQ(res1, res2);
}