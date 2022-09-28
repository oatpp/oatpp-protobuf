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

#include "DynamicObject.hpp"

namespace oatpp { namespace protobuf { namespace reflection {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Class | PolymorphicDispatcher

DynamicClass::PolymorphicDispatcher::PolymorphicDispatcher(DynamicClass* clazz)
  : m_class(clazz)
{}

oatpp::Void DynamicClass::PolymorphicDispatcher::createObject() const {
  auto proto = m_class->createProto();
  auto ptr = std::shared_ptr<DynamicObject>(new DynamicObject(m_class));
  ptr->initFromProto(*proto);
  return oatpp::Void(ptr, m_class->getType());
}

const oatpp::data::mapping::type::BaseObject::Properties* DynamicClass::PolymorphicDispatcher::getProperties() const {

  std::lock_guard<std::mutex> lock(m_class->m_typeMutex);

  if(m_class->m_properties == nullptr) {

    auto proto = m_class->createProto();
    auto objPtr = std::shared_ptr<DynamicObject>(new DynamicObject(m_class));
    objPtr->initFromProto(*proto);
    m_class->m_properties = new oatpp::data::mapping::type::BaseObject::Properties();

    const google::protobuf::Descriptor* desc = proto->GetDescriptor();

    int fieldCount = desc->field_count();
    if(fieldCount != objPtr->m_fields.size()) {
      throw std::runtime_error("[oatpp::protobuf::reflection::DynamicClass::propertiesGetter()]: Error."
                               "Invalid state.");
    }

    for(int i = 0; i < fieldCount; i++) {
      const google::protobuf::FieldDescriptor* field = desc->field(i);
      auto& objField = objPtr->m_fields[i];
      auto prop = new oatpp::data::mapping::type::BaseObject::Property(i * sizeof(oatpp::Void), field->name().c_str(), objField.getValueType());
      m_class->m_properties->pushBack(prop);
    }

  }

  return m_class->m_properties;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Class | VectorPolymorphicDispatcher

DynamicClass::VectorPolymorphicDispatcher::VectorPolymorphicDispatcher(DynamicClass* clazz)
  : m_class(clazz)
{}

oatpp::Void DynamicClass::VectorPolymorphicDispatcher::createObject() const {
  return oatpp::Void(std::make_shared<std::vector<AbstractDynamicObject>>(), m_class->getVectorType());
}

const oatpp::Type *DynamicClass::VectorPolymorphicDispatcher::getItemType() const {
  return m_class->getType();
}

v_int64 DynamicClass::VectorPolymorphicDispatcher::getCollectionSize(const oatpp::Void &object) const {
  const auto &vector = object.cast<oatpp::Vector<AbstractDynamicObject>>();
  return vector->size();
}

void DynamicClass::VectorPolymorphicDispatcher::addItem(const oatpp::Void& object, const oatpp::Void& item) const {

  const auto &vector = object.cast<oatpp::Vector<AbstractDynamicObject>>();
  AbstractDynamicObject vectorItem(std::static_pointer_cast<DynamicObject>(item.getPtr()), m_class->getType());
  vector->push_back(vectorItem);
}

std::unique_ptr<oatpp::data::mapping::type::__class::Collection::Iterator> DynamicClass::VectorPolymorphicDispatcher::beginIteration(const oatpp::Void &object) const {
  using Iterator = oatpp::data::mapping::type::__class::StandardCollection<std::vector<AbstractDynamicObject>, AbstractDynamicObject, void>::Iterator;
  const auto &vector = object.cast<oatpp::Vector<AbstractDynamicObject>>();
  std::unique_ptr<Iterator> iterator(new Iterator());
  iterator->iterator = vector->begin();
  iterator->end = vector->end();
  return iterator;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Class

std::mutex DynamicClass::REGISTRY_MUTEX;
std::unordered_map<std::string, DynamicClass*> DynamicClass::REGISTRY;

DynamicClass::DynamicClass(const std::string& name)
  : m_name(name)
  , m_type(nullptr)
  , m_properties(nullptr)
  , m_vectorType(nullptr)
{}

DynamicClass* DynamicClass::registryGetClass(const std::string& name) {
  std::lock_guard<std::mutex> lock(REGISTRY_MUTEX);
  auto it = REGISTRY.find(name);
  if(it == REGISTRY.end()) {
    auto clazz = new DynamicClass(name);
    REGISTRY[name] = clazz;
    return clazz;
  }
  return it->second;
}

const std::string DynamicClass::getName() const {
  return m_name;
}

std::shared_ptr<Message> DynamicClass::createProto() const {

  const google::protobuf::DescriptorPool* pool = google::protobuf::DescriptorPool::generated_pool();
  const google::protobuf::Descriptor* desc = pool->FindMessageTypeByName(m_name);

  if(desc == nullptr) {
    throw std::runtime_error("[oatpp::protobuf::reflection::DynamicClass::createProto()]: "
                             "Error. Can't find protobuf::Descriptor for name " + m_name);
  }

  return std::shared_ptr<google::protobuf::Message>(
    google::protobuf::MessageFactory::generated_factory()->GetPrototype(desc)->New()
  );

}

const oatpp::Type* DynamicClass::getType() {
  std::lock_guard<std::mutex> lock(m_typeMutex);
  if(m_type == nullptr) {
    Type::Info info;
    info.nameQualifier = m_name.c_str();
    info.polymorphicDispatcher = new PolymorphicDispatcher(this);
    m_type = new oatpp::Type(
      oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID, info
    );
  }
  return m_type;
}

const oatpp::Type* DynamicClass::getVectorType() {
  std::lock_guard<std::mutex> lock(m_typeVectorMutex);
  if(m_vectorType == nullptr) {
    Type::Info info;
    info.polymorphicDispatcher = new VectorPolymorphicDispatcher(this);
    info.params.push_back(getType());
    info.isCollection = true;
    info.parent = Vector<AbstractDynamicObject>::Class::getType();
    m_vectorType = new oatpp::Type(
      oatpp::data::mapping::type::__class::AbstractVector::CLASS_ID,
      info
    );
  }
  return m_vectorType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Object

oatpp::Void DynamicObject::protoValueToOatppValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {

  switch(field->type()) {

    case google::protobuf::FieldDescriptor::TYPE_STRING:
    case google::protobuf::FieldDescriptor::TYPE_BYTES: return Utils::getProtoField<std::string>(refl, field, proto);

    case google::protobuf::FieldDescriptor::TYPE_INT32:
    case google::protobuf::FieldDescriptor::TYPE_SINT32:
    case google::protobuf::FieldDescriptor::TYPE_SFIXED32: return Utils::getProtoField<v_int32>(refl, field, proto);

    case google::protobuf::FieldDescriptor::TYPE_UINT32:
    case google::protobuf::FieldDescriptor::TYPE_FIXED32: return Utils::getProtoField<v_uint32>(refl, field, proto);

    case google::protobuf::FieldDescriptor::TYPE_INT64:
    case google::protobuf::FieldDescriptor::TYPE_SINT64:
    case google::protobuf::FieldDescriptor::TYPE_SFIXED64: return Utils::getProtoField<v_int64>(refl, field, proto);

    case google::protobuf::FieldDescriptor::TYPE_UINT64:
    case google::protobuf::FieldDescriptor::TYPE_FIXED64: return Utils::getProtoField<v_uint64>(refl, field, proto);

    case google::protobuf::FieldDescriptor::TYPE_FLOAT: return Utils::getProtoField<v_float32>(refl, field, proto);
    case google::protobuf::FieldDescriptor::TYPE_DOUBLE: return Utils::getProtoField<v_float64>(refl, field, proto);

    case google::protobuf::FieldDescriptor::TYPE_BOOL: return Utils::getProtoField<bool>(refl, field, proto);

    case google::protobuf::FieldDescriptor::TYPE_ENUM: return Utils::getProtoField<EnumDescriptor>(refl, field, proto);

    case google::protobuf::FieldDescriptor::TYPE_MESSAGE: return Utils::getProtoField<Message>(refl, field, proto);

    // case google::protobuf::FieldDescriptor::TYPE_GROUP: deprecated

    default:
      throw std::runtime_error("[oatpp::protobuf::reflection::DynamicObject::protoValueToOatppValue()]: "
                               "Error. Unknown type - " + std::string(field->type_name()));
  }

}

void DynamicObject::OatppValueToProtoValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, int index) const {

  const auto& value = m_fields[index];
  if(!value) return;

  switch(field->type()) {

    case google::protobuf::FieldDescriptor::TYPE_STRING:
    case google::protobuf::FieldDescriptor::TYPE_BYTES: {
      Utils::setProtoField<std::string>(refl, field, proto, value);
      break;
    }

    case google::protobuf::FieldDescriptor::TYPE_INT32:
    case google::protobuf::FieldDescriptor::TYPE_SINT32:
    case google::protobuf::FieldDescriptor::TYPE_SFIXED32: {
      Utils::setProtoField<v_int32>(refl, field, proto, value);
      break;
    }
    case google::protobuf::FieldDescriptor::TYPE_UINT32:
    case google::protobuf::FieldDescriptor::TYPE_FIXED32: {
      Utils::setProtoField<v_uint32>(refl, field, proto, value);
      break;
    }

    case google::protobuf::FieldDescriptor::TYPE_INT64:
    case google::protobuf::FieldDescriptor::TYPE_SINT64:
    case google::protobuf::FieldDescriptor::TYPE_SFIXED64: {
      Utils::setProtoField<v_int64>(refl, field, proto, value);
      break;
    }
    case google::protobuf::FieldDescriptor::TYPE_UINT64:
    case google::protobuf::FieldDescriptor::TYPE_FIXED64: {
      Utils::setProtoField<v_uint64>(refl, field, proto, value);
      break;
    }

    case google::protobuf::FieldDescriptor::TYPE_FLOAT: {
      Utils::setProtoField<v_float32>(refl, field, proto, value);
      break;
    }
    case google::protobuf::FieldDescriptor::TYPE_DOUBLE: {
      Utils::setProtoField<v_float64>(refl, field, proto, value);
      break;
    }

    case google::protobuf::FieldDescriptor::TYPE_BOOL: {
      Utils::setProtoField<bool>(refl, field, proto, value);
      break;
    }

    case google::protobuf::FieldDescriptor::TYPE_ENUM: {
      Utils::setProtoField<EnumDescriptor>(refl, field, proto, value);
      break;
    }

    case google::protobuf::FieldDescriptor::TYPE_MESSAGE: {
      Utils::setProtoField<Message>(refl, field, proto, value);
      break;
    }

    // case google::protobuf::FieldDescriptor::TYPE_GROUP: deprecated

    default:
      throw std::runtime_error("[oatpp::protobuf::reflection::DynamicObject::protoValueToOatppValue()]: "
                               "Error. Unknown type - " + std::string(field->type_name()));
  }

}

DynamicObject::DynamicObject(DynamicClass* clazz)
  : m_class(clazz)
{}

void DynamicObject::initFromProto(const google::protobuf::Message& proto) {

  const google::protobuf::Descriptor* desc = proto.GetDescriptor();
  const google::protobuf::Reflection* refl = proto.GetReflection();

  int fieldCount = desc->field_count();

  for(int i = 0; i < fieldCount; i++) {
    const google::protobuf::FieldDescriptor* field = desc->field(i);
    m_fields.push_back(protoValueToOatppValue(refl, field, proto));
  }

  setBasePointer(m_fields.data());

}

std::shared_ptr<DynamicObject> DynamicObject::createShared(const google::protobuf::Message& proto) {
  const google::protobuf::Descriptor* desc = proto.GetDescriptor();
  auto clazz = DynamicClass::registryGetClass(desc->full_name());
  auto ptr = std::shared_ptr<DynamicObject>(new DynamicObject(clazz));
  ptr->initFromProto(proto);
  return ptr;
}

void DynamicObject::cloneToProto(google::protobuf::Message& proto) const {

  const google::protobuf::Descriptor* desc = proto.GetDescriptor();
  const google::protobuf::Reflection* refl = proto.GetReflection();

  int fieldCount = desc->field_count();

  if(fieldCount != m_fields.size()) {
    throw std::runtime_error("[oatpp::protobuf::reflection::DynamicObject::toProto()]: Error."
                             "Invalid state.");
  }

  for(int i = 0; i < fieldCount; i++) {
    const google::protobuf::FieldDescriptor* field = desc->field(i);
    OatppValueToProtoValue(refl, field, &proto, i);
  }

}

std::shared_ptr<google::protobuf::Message> DynamicObject::toProto() const {
  auto proto = m_class->createProto();
  cloneToProto(*proto);
  return proto;
}

DynamicClass* DynamicObject::getClass() const {
  return m_class;
}

}}}
