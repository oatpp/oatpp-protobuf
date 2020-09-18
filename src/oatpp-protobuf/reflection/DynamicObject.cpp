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
// Dynamic Class

std::mutex DynamicClass::REGISTRY_MUTEX;
std::unordered_map<std::string, DynamicClass*> DynamicClass::REGISTRY;

DynamicClass::DynamicClass(const std::string& name)
  : m_name(name)
  , m_type(nullptr)
  , m_properties(nullptr)
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

oatpp::Void DynamicClass::creator(DynamicClass* clazz) {
  auto proto = clazz->createProto();
  auto ptr = std::shared_ptr<DynamicObject>(new DynamicObject(clazz));
  ptr->initFromProto(*proto);
  return oatpp::Void(ptr, clazz->getType());
}

oatpp::Type::Properties* DynamicClass::propertiesGetter(DynamicClass* clazz) {
  std::lock_guard<std::mutex> lock(clazz->m_mutex);
  if(clazz->m_properties == nullptr) {

    auto proto = clazz->createProto();
    auto objPtr = std::shared_ptr<DynamicObject>(new DynamicObject(clazz));
    objPtr->initFromProto(*proto);
    clazz->m_properties = new oatpp::Type::Properties();

    const google::protobuf::Descriptor* desc = proto->GetDescriptor();

    int fieldCount = desc->field_count();
    if(fieldCount != objPtr->m_fields.size()) {
      throw std::runtime_error("[oatpp::protobuf::reflection::DynamicClass::propertiesGetter()]: Error."
                               "Invalid state.");
    }

    for(int i = 0; i < fieldCount; i++) {
      const google::protobuf::FieldDescriptor* field = desc->field(i);
      auto& objField = objPtr->m_fields[i];
      oatpp::Type::Property* prop = new oatpp::Type::Property(i * sizeof(oatpp::Void), field->name().c_str(), objField.valueType);
      clazz->m_properties->pushBack(prop);
    }

  }
  return clazz->m_properties;
}

const std::string DynamicClass::getName() const {
  return m_name;
}

std::shared_ptr<google::protobuf::Message> DynamicClass::createProto() const {

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

oatpp::Type* DynamicClass::getType() {
  std::lock_guard<std::mutex> lock(m_mutex);
  if(m_type == nullptr) {
    m_type = new oatpp::Type(
      oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID,
      m_name.c_str(),
      std::bind(creator, this),
      std::bind(propertiesGetter, this),
      nullptr,
      {}
    );
  }
  return m_type;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Object

oatpp::Void DynamicObject::protoValueToOatppValue(const google::protobuf::Reflection* refl,
                                                  const google::protobuf::FieldDescriptor* field,
                                                  const google::protobuf::Message& proto)
{

  switch(field->type()) {

    case google::protobuf::FieldDescriptor::TYPE_STRING: {
      const auto& str = refl->GetString(proto, field);
      return oatpp::String(str.data(), str.size(), true);
    }

    case google::protobuf::FieldDescriptor::TYPE_BYTES: {
      const auto& str = refl->GetString(proto, field);
      return oatpp::String(str.data(), str.size(), true);
    }

    case google::protobuf::FieldDescriptor::TYPE_INT32: return oatpp::Int32(refl->GetInt32(proto, field));
    case google::protobuf::FieldDescriptor::TYPE_SINT32: return oatpp::Int32(refl->GetInt32(proto, field));
    case google::protobuf::FieldDescriptor::TYPE_SFIXED32: return oatpp::Int32(refl->GetInt32(proto, field));
    case google::protobuf::FieldDescriptor::TYPE_UINT32: return oatpp::UInt32(refl->GetUInt32(proto, field));
    case google::protobuf::FieldDescriptor::TYPE_FIXED32: return oatpp::UInt32(refl->GetUInt32(proto, field));

    case google::protobuf::FieldDescriptor::TYPE_INT64: return oatpp::Int64(refl->GetInt64(proto, field));
    case google::protobuf::FieldDescriptor::TYPE_SINT64: return oatpp::Int64(refl->GetInt64(proto, field));
    case google::protobuf::FieldDescriptor::TYPE_SFIXED64: return oatpp::Int64(refl->GetInt64(proto, field));
    case google::protobuf::FieldDescriptor::TYPE_UINT64: return oatpp::UInt64(refl->GetUInt64(proto, field));
    case google::protobuf::FieldDescriptor::TYPE_FIXED64: return oatpp::UInt64(refl->GetUInt64(proto, field));

    case google::protobuf::FieldDescriptor::TYPE_FLOAT: return oatpp::Float32(refl->GetFloat(proto, field));
    case google::protobuf::FieldDescriptor::TYPE_DOUBLE: return oatpp::Float64(refl->GetDouble(proto, field));

    case google::protobuf::FieldDescriptor::TYPE_BOOL: return oatpp::Boolean(refl->GetBool(proto, field));

    case google::protobuf::FieldDescriptor::TYPE_ENUM: {
      const google::protobuf::EnumValueDescriptor* evd = refl->GetEnum(proto, field);
      const auto& name = evd->name();
      return oatpp::String(name.data(), name.size(), true);
    }

    case google::protobuf::FieldDescriptor::TYPE_MESSAGE: {
      auto ptr = DynamicObject::createShared(refl->GetMessage(proto, field));
      return oatpp::Void(ptr, ptr->getClass()->getType());
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

DynamicClass* DynamicObject::getClass() const {
  return m_class;
}

}}}
