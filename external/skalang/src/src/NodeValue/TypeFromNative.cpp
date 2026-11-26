#include "TypeFromNative.h"

const ska::Type ska::TypeFromNative<long>::value() { return ska::Type::MakeBuiltIn<ska::ExpressionType::INT>(); }
const ska::Type ska::TypeFromNative<float>::value() { return ska::Type::MakeBuiltIn<ska::ExpressionType::FLOAT>(); }
const ska::Type ska::TypeFromNative<double>::value() { return ska::Type::MakeBuiltIn<ska::ExpressionType::FLOAT>(); }
const ska::Type ska::TypeFromNative<bool>::value() { return ska::Type::MakeBuiltIn<ska::ExpressionType::BOOLEAN>(); }
const ska::Type ska::TypeFromNative<ska::StringShared>::value() { return ska::Type::MakeBuiltIn<ska::ExpressionType::STRING>(); }
const ska::Type ska::TypeFromNative<ska::NodeValueArray>::value() { return ska::Type::MakeBuiltIn<ExpressionType::ARRAY>(); }