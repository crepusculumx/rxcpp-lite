//
// Created by crepusculumx on 22-5-10.
//


#include "test.h"

#include "rxcpp-lite/observable.hpp"

/**
 * 测试通过source函数构造的Observable，是否能够unsubscribe
 */
TEST(SubscriptionTest, Observalbe) {
  bool clean_flag = false;

  auto observable = rxcpp_lite::Observable<int>(
      [&clean_flag](rxcpp_lite::Subscriber<int> subscriber) {
        subscriber.complete();
        return [&clean_flag]() { clean_flag = true; };
      }
  );

  auto subscription = observable.subscribe(rxcpp_lite::Observer<int>());

  subscription.unsubscribe();
  EXPECT_TRUE(clean_flag);
}