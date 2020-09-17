
#include "oatpp-test/UnitTest.hpp"

#include "oatpp-protobuf/Object.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"


#include "test.pb.h"

#include <iostream>

namespace {

class Test : public oatpp::test::UnitTest {
public:
  Test() : oatpp::test::UnitTest("MyTag")
  {}

  void onRun() override {

    oatpp::protobuf::Object<test::ImageRotateRequest> image = std::make_shared<test::ImageRotateRequest>();

    image->set_rotation(test::ImageRotateRequest_Rotation_NINETY_DEG);
    image->mutable_image()->set_width(320);
    image->mutable_image()->set_height(240);

    oatpp::parser::json::mapping::ObjectMapper mapper;

    {
      auto config = mapper.getSerializer()->getConfig();
      config->enableInterpretations = {"protobuf"};
      config->useBeautifier = true;
    }

    {
      auto config = mapper.getDeserializer()->getConfig();
      config->enableInterpretations = {"protobuf"};
    }

    auto json = mapper.writeToString(image);

    OATPP_LOGD(TAG, "json='%s'", json->c_str());

  }
};

void runTests() {
  OATPP_RUN_TEST(Test);
}

}

int main() {

  oatpp::base::Environment::init();

  runTests();

  /* Print how much objects were created during app running, and what have left-probably leaked */
  /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
  std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

  OATPP_ASSERT(oatpp::base::Environment::getObjectsCount() == 0);

  oatpp::base::Environment::destroy();

  return 0;
}
