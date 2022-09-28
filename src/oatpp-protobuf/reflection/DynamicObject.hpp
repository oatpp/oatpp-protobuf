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

typedef oatpp::data::mapping::type::ObjectWrapper<DynamicObject, oatpp::Void::Class> AbstractDynamicObject;

/**
 * A dynamic class that will be automatically generated for the proto object.
 */
class DynamicClass {
  friend DynamicObject;
private:
  static std::mutex REGISTRY_MUTEX;
  static std::unordered_map<std::string, DynamicClass*> REGISTRY;
public:

  /**
   * Polymorphic Dispatcher
   */
  class PolymorphicDispatcher : public oatpp::data::mapping::type::__class::AbstractObject::PolymorphicDispatcher {
  private:
    DynamicClass* m_class;
  public:

    PolymorphicDispatcher(DynamicClass* clazz);

    oatpp::Void createObject() const override;
    const oatpp::data::mapping::type::BaseObject::Properties* getProperties() const override;
  };

public:

  /**
   * Vector Polymorphic Dispatcher
   */
  class VectorPolymorphicDispatcher : public oatpp::data::mapping::type::__class::Collection::PolymorphicDispatcher {
  private:
    DynamicClass* m_class;
  public:
    VectorPolymorphicDispatcher(DynamicClass* clazz);

    oatpp::Void createObject() const override;
    const oatpp::Type* getItemType() const override;
    void addItem(const oatpp::Void& object, const oatpp::Void& item) const override;
    v_int64 getCollectionSize(const oatpp::Void& object) const override;
    std::unique_ptr<oatpp::data::mapping::type::__class::Collection::Iterator> beginIteration(const oatpp::Void& object) const override;
  };

private:
  std::mutex m_typeMutex;
  std::mutex m_typeVectorMutex;
  std::string m_name;
  oatpp::Type* m_type;
  oatpp::data::mapping::type::BaseObject::Properties* m_properties;
  oatpp::Type* m_vectorType;
private:
  DynamicClass(const std::string& name);
public:

  /**
   * Get class by name of the proto object type.
   * @param name
   * @return
   */
  static DynamicClass* registryGetClass(const std::string& name);

  /**
   * Get class for proto object type.
   * @tparam T
   * @return
   */
  template<class T>
  static DynamicClass* registryGetClass() {
    const google::protobuf::Descriptor* desc = T::GetDescriptor();
    return registryGetClass(desc->full_name());
  }

  /**
   * Get class name.
   * @return
   */
  const std::string getName() const;

  /**
   * Instantiate shared proto object.
   * @return
   */
  std::shared_ptr<Message> createProto() const;

  /**
   * Get &id:oatpp::Type; of this class.
   * @return
   */
  const oatpp::Type* getType();

  /**
   * Get &id:oatpp::Type; of `oatpp::Vector<This-Class>`
   * @return
   */
  const oatpp::Type* getVectorType();

};

/**
 * A dynamic oatpp object that will be created from the proto object.
 */
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

  /**
   * Create shared.
   * @param proto
   * @return
   */
  static std::shared_ptr<DynamicObject> createShared(const Message& proto);

  void cloneToProto(Message& proto) const;
  std::shared_ptr<Message> toProto() const;

  /**
   * Get a &l:DynamicClass; of this object.
   * @return
   */
  DynamicClass* getClass() const;

};

template<>
struct TypeHelper <Message> {

  typedef Message CT;
  typedef AbstractDynamicObject StaticType;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const StaticType& value) {
    DynamicObject* obj = static_cast<DynamicObject*>(value.get());
    auto message = refl->MutableMessage(proto, field);
    obj->cloneToProto(*message);
  }

  static StaticType getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    auto ptr = DynamicObject::createShared(refl->GetMessage(proto, field));
    return StaticType(ptr, ptr->getClass()->getType());
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const StaticType& value) {
    DynamicObject* obj = static_cast<DynamicObject*>(value.get());
    auto message = refl->AddMessage(proto, field);
    obj->cloneToProto(*message);
  }

  static StaticType getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    auto ptr = DynamicObject::createShared(refl->GetRepeatedMessage(proto, field, index));
    return StaticType(ptr, ptr->getClass()->getType());
  }

  static const oatpp::Type* getDynamicType(const FieldDescriptor* field) {
    auto className = field->message_type()->full_name();
    return DynamicClass::registryGetClass(className)->getType();
  }

  static const oatpp::Type* getDynamicVectorType(const FieldDescriptor* field) {
    auto className = field->message_type()->full_name();
    return DynamicClass::registryGetClass(className)->getVectorType();
  }

};

}}}

#endif // oatpp_protobuf_reflection_DynamicObject_hpp
