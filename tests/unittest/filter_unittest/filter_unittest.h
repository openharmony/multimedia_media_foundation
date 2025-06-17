/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FILTER_UNITTEST_H
#define FILTER_UNITTEST_H

#include "gtest/gtest.h"
#include "filter.h"

namespace OHOS {
namespace Media {
namespace Pipeline {

class FilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp(void);
    void TearDown(void);
    std::shared_ptr<Filter> filter_;
};

class TestFilter : public Filter {
public:
    explicit TestFilter(std::string name, FilterType type)
        : Filter(name, type) {}
    Status DoFreeze() override
    {
        return Status::ERROR_UNKNOWN;
    }

    Status DoUnFreeze() override
    {
        return Status::ERROR_UNKNOWN;
    }

    Status DoPauseDragging() override
    {
        return Status::ERROR_UNKNOWN;
    }

    Status DoPauseAudioAlign() override
    {
        return Status::ERROR_UNKNOWN;
    }

    Status DoResumeDragging() override
    {
        return Status::ERROR_UNKNOWN;
    }

    Status DoResumeAudioAlign() override
    {
        return Status::ERROR_UNKNOWN;
    }

    Status DoPreroll() override
    {
        return Status::ERROR_UNKNOWN;
    }

    Status DoWaitPrerollDone(bool render) override
    {
        return Status::ERROR_UNKNOWN;
    }
};

} // namespace Pipeline
} // namespace Media
} // namespace OHOS
#endif // FILTER_UNITTEST_H