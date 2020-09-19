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

#ifndef oatpp_protobuf_reflection_Utils_hpp
#define oatpp_protobuf_reflection_Utils_hpp

#include "oatpp/core/Types.hpp"
#include <google/protobuf/Message.h>

namespace oatpp { namespace protobuf { namespace reflection {

typedef google::protobuf::Reflection Reflection;
typedef google::protobuf::FieldDescriptor FieldDescriptor;
typedef google::protobuf::EnumDescriptor EnumDescriptor;
typedef google::protobuf::Message Message;

template<typename T>
struct TypeHelper {
};

class Utils {
public:

  template<typename CT>
  static oatpp::Void getProtoField(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {

    if (field->is_repeated()) {
      oatpp::Vector<typename TypeHelper<CT>::OT> arr({});
      int size = refl->FieldSize(proto, field);
      for (int i = 0; i < size; i++) {
        arr->push_back(TypeHelper<CT>::getArrayItem(refl, field, proto, i));
      }
      return arr;
    } else if(refl->HasField(proto, field)) {
      return TypeHelper<CT>::getFieldValue(refl, field, proto);
    }
    return oatpp::Void(nullptr, TypeHelper<CT>::getOType(refl, field, proto));

  }

  template<typename CT>
  static void setProtoField(const Reflection* refl, const FieldDescriptor* field, Message* proto, const oatpp::Void& value) {

    if(field->is_repeated()) {
      const auto& arr = value.staticCast<oatpp::Vector<typename TypeHelper<CT>::OT>>();
      refl->ClearField(proto, field);
      for(auto& val : *arr) {
        TypeHelper<CT>::addArrayItem(refl, field, proto, val);
      }
    } else {
      const auto& val = value.staticCast<typename TypeHelper<CT>::OT>();
      TypeHelper<CT>::setFieldValue(refl, field, proto, val);
    }

  }

};

template<>
struct TypeHelper <std::string> {

  typedef std::string CT;
  typedef oatpp::String OT;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->SetString(proto, field, value->std_str());
  }

  static OT getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    const auto& str = refl->GetString(proto, field);
    return OT(str.data(), str.size(), true);
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->AddString(proto, field, value->std_str());
  }

  static OT getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    const auto& str = refl->GetRepeatedString(proto, field, index);
    return OT(str.data(), str.size(), true);
  }

  static const oatpp::Type* getOType(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    (void) refl;
    (void) field;
    (void) proto;
    return OT::Class::getType();
  }

};

template<>
struct TypeHelper <v_int32> {

  typedef v_int32 CT;
  typedef oatpp::Int32 OT;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->SetInt32(proto, field, *value);
  }

  static OT getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    return refl->GetInt32(proto, field);
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->AddInt32(proto, field, *value);
  }

  static OT getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    return refl->GetRepeatedInt32(proto, field, index);
  }

  static const oatpp::Type* getOType(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    (void) refl;
    (void) field;
    (void) proto;
    return OT::Class::getType();
  }

};

template<>
struct TypeHelper <v_uint32> {

  typedef v_uint32 CT;
  typedef oatpp::UInt32 OT;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->SetUInt32(proto, field, *value);
  }

  static OT getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    return refl->GetUInt32(proto, field);
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->AddUInt32(proto, field, *value);
  }

  static OT getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    return refl->GetRepeatedUInt32(proto, field, index);
  }

  static const oatpp::Type* getOType(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    (void) refl;
    (void) field;
    (void) proto;
    return OT::Class::getType();
  }

};

template<>
struct TypeHelper <v_int64> {

  typedef v_int64 CT;
  typedef oatpp::Int64 OT;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->SetInt64(proto, field, *value);
  }

  static OT getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    return refl->GetInt64(proto, field);
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->AddInt64(proto, field, *value);
  }

  static OT getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    return refl->GetRepeatedInt64(proto, field, index);
  }

  static const oatpp::Type* getOType(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    (void) refl;
    (void) field;
    (void) proto;
    return OT::Class::getType();
  }

};

template<>
struct TypeHelper <v_uint64> {

  typedef v_uint64 CT;
  typedef oatpp::UInt64 OT;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->SetUInt64(proto, field, *value);
  }

  static OT getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    return refl->GetUInt64(proto, field);
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->AddUInt64(proto, field, *value);
  }

  static OT getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    return refl->GetRepeatedUInt64(proto, field, index);
  }

  static const oatpp::Type* getOType(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    (void) refl;
    (void) field;
    (void) proto;
    return OT::Class::getType();
  }

};

template<>
struct TypeHelper <v_float32> {

  typedef v_float32 CT;
  typedef oatpp::Float32 OT;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->SetFloat(proto, field, *value);
  }

  static OT getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    return refl->GetFloat(proto, field);
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->AddFloat(proto, field, *value);
  }

  static OT getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    return refl->GetRepeatedFloat(proto, field, index);
  }

  static const oatpp::Type* getOType(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    (void) refl;
    (void) field;
    (void) proto;
    return OT::Class::getType();
  }

};

template<>
struct TypeHelper <v_float64> {

  typedef v_float64 CT;
  typedef oatpp::Float64 OT;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->SetDouble(proto, field, *value);
  }

  static OT getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    return refl->GetDouble(proto, field);
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->AddDouble(proto, field, *value);
  }

  static OT getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    return refl->GetRepeatedDouble(proto, field, index);
  }

  static const oatpp::Type* getOType(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    (void) refl;
    (void) field;
    (void) proto;
    return OT::Class::getType();
  }

};

template<>
struct TypeHelper <bool> {

  typedef bool CT;
  typedef oatpp::Boolean OT;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->SetBool(proto, field, *value);
  }

  static OT getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    return refl->GetBool(proto, field);
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    refl->AddBool(proto, field, *value);
  }

  static OT getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    return refl->GetRepeatedBool(proto, field, index);
  }

  static const oatpp::Type* getOType(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    (void) refl;
    (void) field;
    (void) proto;
    return OT::Class::getType();
  }

};

template<>
struct TypeHelper <EnumDescriptor> {

  typedef EnumDescriptor CT;
  typedef oatpp::String OT;

  static void setFieldValue(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    const auto& val = value.staticCast<oatpp::String>();
    const google::protobuf::EnumDescriptor* ed = field->enum_type();
    refl->SetEnum(proto, field, ed->FindValueByName(val->std_str()));
  }

  static OT getFieldValue(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    const google::protobuf::EnumValueDescriptor* evd = refl->GetEnum(proto, field);
    const auto& name = evd->name();
    return oatpp::String(name.data(), name.size(), true);
  }

  static void addArrayItem(const Reflection* refl, const FieldDescriptor* field, Message* proto, const OT& value) {
    const auto& val = value.staticCast<oatpp::String>();
    const google::protobuf::EnumDescriptor* ed = field->enum_type();
    refl->AddEnum(proto, field, ed->FindValueByName(val->std_str()));
  }

  static OT getArrayItem(const Reflection* refl, const FieldDescriptor* field, const Message& proto, int index) {
    const google::protobuf::EnumValueDescriptor* evd = refl->GetRepeatedEnum(proto, field, index);
    const auto& name = evd->name();
    return oatpp::String(name.data(), name.size(), true);
  }

  static const oatpp::Type* getOType(const Reflection* refl, const FieldDescriptor* field, const Message& proto) {
    (void) refl;
    (void) field;
    (void) proto;
    return OT::Class::getType();
  }

};

}}}

#endif // oatpp_protobuf_reflection_Utils_hpp
