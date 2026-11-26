#include "Config/LoggerConfigLang.h"

#include "Symbol.h"
#include "Type.h"
#include "Service/SymbolTable.h"
#include "Service/TypeCrosser/TypeCrossExpression.h"
#include "Runtime/Service/ScriptTypeSerializer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Type)

ska::Type ska::Type::crossTypes(const TypeCrosser& crosser, std::string op, const Type& type2) const {
	return crosser.cross(op, *this, type2);
}

bool ska::Type::isAssignableFrom(const Type& type) const {
	if (*this == type) {
		return true;
	}
	const auto* currentRefClass = m_type == ExpressionType::FUNCTION ? this : getParentClass();
	const auto* typeRefClass = type == ExpressionType::FUNCTION ? &type : type.getParentClass();
	if (typeRefClass == nullptr || currentRefClass == nullptr) {
		// Only assignable if both have no parent class (dereferencing the null one crashed when only one had a parent class)
		return typeRefClass == currentRefClass;
	}
	return *typeRefClass == *currentRefClass;
}

ska::Type::Type(const ScopedSymbolTable* symbolTable, ExpressionType t) :
	m_type(t),
	m_symbolTable(symbolTable == nullptr || symbolTable->symbol() == nullptr ? nullptr : symbolTable) {
}

const ska::Type *ska::Type::getParentClass() const {
	if (m_symbolTable == nullptr) {
		return nullptr;
	}

	if (m_symbolTable->symbol()->type() == ExpressionType::FUNCTION) {
		return &m_symbolTable->symbol()->type();
	}

	if (m_symbolTable->classTable() == nullptr) {
		return nullptr;
	}

	const auto& parentType = m_symbolTable->classTable()->symbol()->type();
	if (parentType == ExpressionType::OBJECT) {
		return parentType.getParentClass();
	}

	assert(parentType == ExpressionType::FUNCTION);
	return &parentType;
}

bool ska::Type::operator==(const Type& t) const {
	if (m_symbolTable != nullptr && t.m_symbolTable != nullptr) {
		return *m_symbolTable->symbol() == *t.m_symbolTable->symbol();
	}

	return structuralEquality(t);
}

std::string ska::Type::name() const {
	return m_symbolTable == nullptr ? "" : m_symbolTable->name();
}

bool ska::Type::structuralEquality(const Type& t) const {
	return m_type == t.m_type && m_compound == t.m_compound;
}

bool ska::Type::tryChangeSymbol(const Type& type) {
	if (type.m_symbolTable != nullptr && m_symbolTable != type.m_symbolTable) {
		m_symbolTable = type.m_symbolTable;
		return true;
	}
	return false;
}

std::ostream& ska::operator<<(std::ostream& stream, const ska::Type& type) {
	const auto mainType = ExpressionTypeSTR[static_cast<std::size_t>(type.m_type)];
	auto addedSymbolPart = (type.m_symbolTable == nullptr ? "" : (" " + type.m_symbolTable->name()));
	if (type.m_compound.empty()) {
		stream << mainType << addedSymbolPart;
	} else {
		stream << mainType << addedSymbolPart << " (";
		std::size_t rindex = type.m_compound.size() - 1;
		for (const auto& childType : type.m_compound) {
			stream << childType << (rindex == 0 ? "" : " - ");
			rindex--;
		}
		stream << ")";
	}

	return stream;
}

void ska::Type::serialize(SerializerOutput& output, ScriptTypeSerializer& serializer, bool writeSymbol) const {
	serializer.write(output, writeSymbol ? m_symbolTable : nullptr, *this);
}

static const ska::Type* AccessUnderlyingClassType(const ska::ScopedSymbolTable* symbolTable) {
	return symbolTable == nullptr ||
		symbolTable->classTable() == nullptr ||
		symbolTable->classTable()->symbol() == nullptr ? nullptr : &symbolTable->classTable()->symbol()->type();
}

bool ska::Type::empty() const{
	if (m_type != ExpressionType::OBJECT || !m_compound.empty()) {
		return m_compound.empty();
	}
	const Type* classType = AccessUnderlyingClassType(m_symbolTable);
	return m_compound.empty() && (classType == nullptr || classType->empty());
}

const ska::Type& ska::Type::back() const {
	if (m_type != ExpressionType::OBJECT || !m_compound.empty()) {
		return m_compound.empty() ? *this : m_compound.back();
	}
	const Type* classType = AccessUnderlyingClassType(m_symbolTable);
	assert(classType != nullptr);
	return classType->back();
}

std::size_t ska::Type::size() const {
	if (m_type != ExpressionType::OBJECT) {
		return m_compound.size();
	}

	const Type* classType = AccessUnderlyingClassType(m_symbolTable);
	return (classType == nullptr ? 0 : classType->size()) + m_compound.size();
}

const ska::Type& ska::Type::operator[](std::size_t index) const {
	if (m_type != ExpressionType::OBJECT || index < m_compound.size()) {
		return m_compound[index];
	}
	const Type* classType = AccessUnderlyingClassType(m_symbolTable);
	assert(classType != nullptr);
	return (*classType)[index - m_compound.size()];
}
