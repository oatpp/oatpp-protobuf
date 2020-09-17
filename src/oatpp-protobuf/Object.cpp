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

#include "Object.hpp"

namespace oatpp { namespace protobuf {

namespace __class {
  const ClassId AbstractObject::CLASS_ID("oatpp::protobuf::AbstractObject");
}

oatpp::Void ProtoUtils::protoValueToOatppValue(const google::protobuf::Reflection* refl,
                                               const google::protobuf::FieldDescriptor *field,
                                               const google::protobuf::Message &proto)
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

    case google::protobuf::FieldDescriptor::TYPE_MESSAGE: return protoToOatppMap(refl->GetMessage(proto, field));

    // case google::protobuf::FieldDescriptor::TYPE_GROUP: deprecated

    default:
      throw std::runtime_error("[oatpp::protobuf::ProtoUtils::protoValueToOatppValue()]: "
                               "Error. Unknown type - " + std::string(field->type_name()));
  }

}

oatpp::Fields<oatpp::Any> ProtoUtils::protoToOatppMap(const google::protobuf::Message& proto) {

  oatpp::Fields<oatpp::Any> result({});

  const google::protobuf::Descriptor* desc = proto.GetDescriptor();
  const google::protobuf::Reflection* refl = proto.GetReflection();

  OATPP_LOGD("AAA", "proto class name='%s'", desc->full_name().c_str());

  int fieldCount = desc->field_count();

  for(int i = 0; i < fieldCount; i++) {
    const google::protobuf::FieldDescriptor* field = desc->field(i);
    result->push_back({field->name().c_str(), protoValueToOatppValue(refl, field, proto)});
  }

  return result;

}

oatpp::Fields<oatpp::Any> ProtoUtils::protoToOatppMap(const std::shared_ptr<google::protobuf::Message>& proto) {
  if(proto) {
    return protoToOatppMap(*proto);
  }
  return nullptr;
}

}}
