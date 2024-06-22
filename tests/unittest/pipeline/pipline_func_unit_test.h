#include <gtest/gtest.h>
#include "pipeline/pipeline.h"
#include "filter/filter.h"
namespace OHOS {
namespace Media {
namespace PiplineFuncUT {
class TestFilter : public Pipeline::Filter {
public:
    TestFilter(std::string name, Pipeline::FilterType type): Pipeline::Filter(std::move(name), type) {};
    ~TestFilter() override = default;
}
class PiplineUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);
private:
    std::shared_ptr<Pipeline::Pipeline> pipeline_;
    std::shared_ptr<TestFilter> filterOne_;
    std::shared_ptr<TestFilter> filterTwo_;
    std::string testId;
};
} // namespace PiplineFuncUT
} // namespace Media
} // namespace OHOS