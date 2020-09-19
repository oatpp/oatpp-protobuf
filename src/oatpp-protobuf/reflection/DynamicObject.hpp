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

#include "Utils.hpp"

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

  std::shared_ptr<Message> createProto() const;

  oatpp::Type* getType();

};

class DynamicObject : public oatpp::BaseObject {
  friend DynamicClass;
private:
  static oatpp::Void protoValueToOatppValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto);
  void OatppValueToProtoValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, int index) const;
private:
  DynamicClass* m_class;
  std::vector<oatpp::Void> m_fields;
private:
  DynamicObject(DynamicClass* clazz);
  void initFromProto(const Message& proto);
public:

  static std::shared_ptr<DynamicObject> createShared(const Message& proto);

  void cloneToProto(Message& proto) const;
  std::shared_ptr<Message> toProto() const;

  DynamicClass* getClass() const;

};

template<>
struct TypeHelper <Message> {

  typedef Message CT;
  //typedef oatpp::data::mapping::type::ObjectWrapper<DynamicObject, > OT;
  typedef oatpp::Void OT;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    DynamicObject* obj = static_cast<DynamicObject*>(value.get());
    auto message = refl->MutableMessage(proto, field);
    obj->cloneToProto(*message);
  }

  static OT getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    auto ptr = DynamicObject::createShared(refl->GetMessage(proto, field));
    return oatpp::Void(ptr, ptr->getClass()->getType());
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    DynamicObject* obj = static_cast<DynamicObject*>(value.get());
    auto message = refl->AddMessage(proto, field);
    obj->cloneToProto(*message);
  }

  static OT getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    auto ptr = DynamicObject::createShared(refl->GetRepeatedMessage(proto, field, index));
    return oatpp::Void(ptr, ptr->getClass()->getType());
  }

  static const oatpp::Type* getOType(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    const auto& m = refl->GetMessage(proto, field);
    const google::protobuf::Descriptor* desc = m.GetDescriptor();
    return DynamicClass::registryGetClass(desc->full_name())->getType();
  }

};

}}}

#endif // oatpp_protobuf_reflection_DynamicObject_hpp
