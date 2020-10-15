
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

    oatpp::protobuf::Object<test::ImageRotateRequest> req = std::make_shared<test::ImageRotateRequest>();


    req->add_rotation(test::ImageRotateRequest_Rotation_NINETY_DEG);
    req->add_rotation(test::ImageRotateRequest_Rotation_ONE_EIGHTY_DEG);

    auto image1 = req->add_image();
    image1->set_data("Hello World!");
    image1->set_width(-1);
    image1->set_height(240);

    req->add_intarr(1);
    req->add_intarr(2);
    req->add_intarr(3);

    oatpp::parser::json::mapping::ObjectMapper mapper;

    {
      auto config = mapper.getSerializer()->getConfig();
      config->enabledInterpretations = {"protobuf"};
      config->useBeautifier = true;
    }

    {
      auto config = mapper.getDeserializer()->getConfig();
      config->enabledInterpretations = {"protobuf"};
    }

    auto json1 = mapper.writeToString(req);
    OATPP_LOGD(TAG, "json1='%s'", json1->c_str());

    auto clone = mapper.readFromString<oatpp::protobuf::Object<test::ImageRotateRequest>>(json1);

    auto json2 = mapper.writeToString(clone);
    OATPP_LOGD(TAG, "json2='%s'", json2->c_str());

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
