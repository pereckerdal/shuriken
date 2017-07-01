// Copyright 2017 Per Grön. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <rs/subscription.h>

namespace shk {
namespace detail {

void EmptySubscription::Request(ElementCount count) {}

void EmptySubscription::Cancel() {}

SubscriptionEraser::~SubscriptionEraser() {}

}  // namespace detail

Subscription::Subscription() {}

void Subscription::Request(ElementCount count) {
  if (eraser_) {
    eraser_->Request(count);
  }
}

void Subscription::Cancel() {
  if (eraser_) {
    eraser_->Cancel();
  }
}

SharedSubscription::SharedSubscription() {}

void SharedSubscription::Request(ElementCount count) {
  if (eraser_) {
    eraser_->Request(count);
  }
}

void SharedSubscription::Cancel() {
  if (eraser_) {
    eraser_->Cancel();
  }
}

WeakSubscription::WeakSubscription() {}

WeakSubscription::WeakSubscription(const SharedSubscription &s)
		: eraser_(s.eraser_) {}

void WeakSubscription::Request(ElementCount count) {
  if (auto eraser = eraser_.lock()) {
    eraser->Request(count);
  }
}

void WeakSubscription::Cancel() {
  if (auto eraser = eraser_.lock()) {
    eraser->Cancel();
  }
}

detail::EmptySubscription MakeSubscription() {
  return detail::EmptySubscription();
}

}  // namespace shk
