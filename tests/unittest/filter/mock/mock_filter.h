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
#ifndef AVCODEC_UNIT_TEST_FILTER_MOCK_FILTER
#define AVCODEC_UNIT_TEST_FILTER_MOCK_FILTER
 
#include "filter/filter.h"
 
namespace OHOS {
namespace Media {
namespace FilterUnitTest {
using namespace OHOS::Media;
using namespace OHOS::Media::Pipeline;
class MockFilterForTest : public Filter {
public:
    explicit MockFilterForTest(std::string name, FilterType type,
        bool asyncMode = false) : Filter(name, type, asyncMode)
    {
        (void)name;
        (void)type;
        (void)asyncMode;
    }
public:
    int32_t testNum_ = 0;
};
} // namespace FilterUnitTest
} // namespace Media
} // namespace OHOS
 
#endif