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

#ifndef oatpp_protobuf_reflection_DynamicObject_hpp
#define oatpp_protobuf_reflection_DynamicObject_hpp

#include "oatpp/core/Types.hpp"

#include <google/protobuf/Message.h>

namespace oatpp { namespace protobuf { namespace reflection {

class DynamicObject; // FWD

class DynamicClass {
  friend DynamicObject;
private:
  static std::mutex REGISTRY_MUTEX;
  static std::unordered_map<std::string, DynamicClass*> REGISTRY;
private:
  std::mutex m_mutex;
  std::string m_name;
  oatpp::Type* m_type;
  oatpp::Type::Properties* m_properties;
private:
  DynamicClass(const std::string& name);
private:
  static oatpp::Void creator(DynamicClass* clazz);
  static oatpp::Type::Properties* propertiesGetter(DynamicClass* clazz);
public:

  static DynamicClass* registryGetClass(const std::string& name);

  template<class T>
  static DynamicClass* registryGetClass() {
    const google::protobuf::Descriptor* desc = T::GetDescriptor();
    return registryGetClass(desc->full_name());
  }

  const std::string getName() const;

  std::shared_ptr<google::protobuf::Message> createProto() const;

  oatpp::Type* getType();

};

class DynamicObject : public oatpp::BaseObject {
  friend DynamicClass;
private:
  static oatpp::Void protoValueToOatppValue(const google::protobuf::Reflection* refl,
                                            const google::protobuf::FieldDescriptor* field,
                                            const google::protobuf::Message& proto);

  void OatppValueToProtoValue(const google::protobuf::Reflection* refl,
                              const google::protobuf::FieldDescriptor* field,
                              google::protobuf::Message* proto,
                              int index) const;
private:
  DynamicClass* m_class;
  std::vector<oatpp::Void> m_fields;
private:
  DynamicObject(DynamicClass* clazz);
  void initFromProto(const google::protobuf::Message& proto);
public:

  static std::shared_ptr<DynamicObject> createShared(const google::protobuf::Message& proto);

  void cloneToProto(google::protobuf::Message& proto) const;
  std::shared_ptr<google::protobuf::Message> toProto() const;

  DynamicClass* getClass() const;

};

}}}

#endif // oatpp_protobuf_reflection_DynamicObject_hpp
