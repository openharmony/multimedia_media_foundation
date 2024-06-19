#include <gtest/gtest.h>
#include "../interface/inner_api/pipeline/pipeline.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
namespace PiplineFuncUT {
class PiplineUnitTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp(void);

    void TearDown(void);
};

class TestFilter : public Filter {

}
} // namespace PiplineFuncUT
} // namespace Media
} // namespace OHOS