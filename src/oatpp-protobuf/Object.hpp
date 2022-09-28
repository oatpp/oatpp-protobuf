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

#include "reflection/DynamicObject.hpp"

namespace oatpp { namespace protobuf {

namespace __class {

  /**
   * Abstract proto object.
   */
  class AbstractObject {
  public:
    static const oatpp::ClassId CLASS_ID;
  };

  template<class T>
  class Object;
}

/**
 * Proto object.
 */
template <class T>
using Object = oatpp::data::mapping::type::ObjectWrapper<T, __class::Object<T>>;

namespace __class {

  template<class T>
  class Object : public AbstractObject {
  private:

    class Inter : public oatpp::Type::AbstractInterpretation {
    public:

      oatpp::Void toInterpretation(const Void& originalValue) const override {
        const auto& value = originalValue.cast<oatpp::protobuf::Object<T>>();
        auto ptr = reflection::DynamicObject::createShared(*value.getPtr());
        return oatpp::Void(ptr, ptr->getClass()->getType());
      }

      oatpp::Void fromInterpretation(const Void& interValue) const override {
        if(interValue) {
          auto obj = static_cast<reflection::DynamicObject*>(interValue.get());
          auto ptr = obj->toProto();
          return oatpp::Void(ptr, Object::getType());
        }
        return nullptr;
      }

      const oatpp::Type* getInterpretationType() const override {
        auto clazz = reflection::DynamicClass::registryGetClass<T>();
        return clazz->getType();
      }

    };

    static oatpp::Type createType() {
        Type::Info info;
        info.interpretationMap["protobuf"] = new Inter();
        return oatpp::Type(
                CLASS_ID,
                info
        );
    }

  public:

    static oatpp::Type* getType(){
      static oatpp::Type type = createType();
      return &type;
    }

  };

}

}}

#endif // oatpp_protobuf_Object_hpp
