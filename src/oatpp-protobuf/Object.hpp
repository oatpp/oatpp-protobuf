/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
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
 *
 ***************************************************************************/

#ifndef oatpp_protobuf_Object_hpp
#define oatpp_protobuf_Object_hpp

#include "oatpp/core/Types.hpp"

#include <google/protobuf/Message.h>

namespace oatpp { namespace protobuf {

namespace __class {

  class AbstractObject {
  public:
    static const oatpp::ClassId CLASS_ID;
  };

  template<class T>
  class Object;
}

class ProtoUtils {
public:
  static oatpp::Void protoValueToOatppValue(const google::protobuf::Reflection* refl,
                                            const google::protobuf::FieldDescriptor *field,
                                            const google::protobuf::Message &proto);

  static oatpp::Fields<oatpp::Any> protoToOatppMap(const google::protobuf::Message &proto);
  static oatpp::Fields<oatpp::Any> protoToOatppMap(const std::shared_ptr<google::protobuf::Message> &proto);
};

template <class T>
using Object = oatpp::data::mapping::type::ObjectWrapper<T, __class::Object<T>>;

namespace __class {

  template<class T>
  class Object : public AbstractObject {
  private:

    class Inter : public oatpp::Type::Interpretation<oatpp::protobuf::Object<T>, oatpp::Fields<oatpp::Any>>  {
    public:

      oatpp::Fields<oatpp::Any> interpret(const oatpp::protobuf::Object<T>& value) const override {
        return ProtoUtils::protoToOatppMap(value.getPtr());
      }

      oatpp::protobuf::Object<T> reproduce(const oatpp::Fields<oatpp::Any>& value) const override {
        return nullptr;
      }

    };

  public:

    static oatpp::Type* getType(){
      static Type type(
        CLASS_ID, nullptr, nullptr, nullptr, nullptr,
        {
          {"protobuf", new Inter()}
        }
      );
      return &type;
    }

  };

}

}}

#endif // oatpp_protobuf_Object_hpp
