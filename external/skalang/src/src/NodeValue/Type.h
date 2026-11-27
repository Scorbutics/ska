#pragma once
#include <unordered_map>
#include <vector>
#include <sstream>
#include <cassert>
#include "ExpressionType.h"

namespace ska {
	class ScopedSymbolTable;
	class SymbolTable;
	class TypeCrosser;
	class SerializerOutput;
	class ScriptTypeSerializer;

	struct Type {
		static constexpr bool isNamed(ExpressionType type) {
			return type == ExpressionType::FUNCTION || type == ExpressionType::OBJECT;
		}

		static constexpr bool isNumeric(ExpressionType type) {
			return type == ExpressionType::INT || type == ExpressionType::FLOAT || type == ExpressionType::BOOLEAN;
		}

		static constexpr bool isBuiltIn(ExpressionType type) {
			return isNumeric(type) || type == ExpressionType::STRING || type == ExpressionType::VOID;
		}

		static bool isNamed(const Type& type) {
			return isNamed(type.m_type);
		}

		static bool isNumeric(const Type& type) {
			return isNumeric(type.m_type);
		}

		static bool isBuiltIn(const Type& type) {
			return isBuiltIn(type.m_type);
		}

		static Type MakeBuiltInArray(ExpressionType t) {
			assert(t != ExpressionType::VOID);
			auto result = Type{ ExpressionType::ARRAY };
			result.m_compound.push_back(Type{ t });
			return result;
		}

		template<ExpressionType t>
		static Type MakeBuiltIn() {
			static_assert(t != ExpressionType::VOID);
			return Type{ t };
		}

		static Type MakeBuiltIn(ExpressionType t, const ScopedSymbolTable* symbolTable = nullptr) {
			return Type{ symbolTable, t };
		}

		template<ExpressionType t>
		static Type MakeCustom(const ScopedSymbolTable* symbolTable) {
			static_assert(isNamed(t));
			return Type{ symbolTable, t };
		}

		static Type Override(Type t, const ScopedSymbolTable* symbolTable) {
			// The symbol table of an enum type is the enum itself (not the symbol of a value of it)
			if (t.m_type == ExpressionType::ENUM && t.m_symbolTable != nullptr) {
				return t;
			}
			t.m_symbolTable = symbolTable;
			return t;
		}

		Type() = default;
		Type(Type&& t) noexcept = default;
		Type(const Type& t) = default;

		Type& operator=(Type&& t) noexcept = default;
		Type& operator=(const Type& t) = default;

		~Type() = default;

		ExpressionType type() const {
			return m_type;
		}
		bool operator==(const Type& t) const;


		bool operator==(const ExpressionType& t) const {
			return m_type == t;
		}

		bool operator!=(const ExpressionType& t) const {
			return m_type != t;
		}
		Type& operator=(ExpressionType t) {
			m_type = std::move(t);
			return *this;
		}

		Type& add(Type t) {
			m_compound.push_back(std::move(t));
			return *this;
		}

		bool operator!=(const Type& t) const {
			return !(*this == t);
		}

		bool structuralEquality(const Type&) const;

		Type crossTypes(const TypeCrosser& crosser, std::string op, const Type& type2) const;

		bool isAssignableFrom(const Type&) const;

		std::size_t size() const;
		bool empty() const;

		const Type& back() const;

		const Type& operator[](std::size_t index) const;

		std::string name() const;
		// Symbol table of a named type (a function or object type declared by a script), nullptr otherwise
		const ScopedSymbolTable* symbolTable() const { return m_symbolTable; }

		bool tryChangeSymbol(const Type& type);

		void serialize(SerializerOutput& output, ScriptTypeSerializer& serializer, bool writeSymbol) const;

	private:
		friend class TypeCrosser;
		friend class TypeHierarchy;

		explicit Type(ExpressionType t) :
			m_type(std::move(t)) {
		}

		Type(const ScopedSymbolTable* symbolTable, ExpressionType t);
		const Type* getParentClass() const;


		ExpressionType m_type = ExpressionType::VOID;
    	const ScopedSymbolTable* m_symbolTable = nullptr;
		std::vector<Type> m_compound;

		friend std::ostream& operator<<(std::ostream& stream, const Type& type);
	};

	std::ostream& operator<<(std::ostream& stream, const Type& type);
}
