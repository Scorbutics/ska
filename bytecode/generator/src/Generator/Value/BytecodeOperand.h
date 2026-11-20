#pragma once
#include <variant>
#include <memory>
#include <tuple>
#include <unordered_map>
#include "Runtime/Value/PlainMemoryTable.h"
#include "NodeValue/Type.h"
#include "Generator/BytecodeCommand.h"
#include "Runtime/Value/StringShared.h"
#include "Runtime/Value/ScriptVariableRef.h"
#include "NodeValue/Cursor.h"

namespace ska {
	class ASTNode;

	namespace bytecode {
		using OperandVariant = std::variant<ScriptVariableRef, long, bool, double, StringShared>;

		enum class OperandType {
			EMPTY,
			PURE,
			VAR,
			REG,
			BIND_NATIVE,
			BIND_SCRIPT,
			MAGIC,
			// Local variable of the current function call frame
			LOCAL,
			// Variable of an enclosing function, captured by the current closure (index in its captured cells)
			UPVAL
		};

		static constexpr const char* OperandTypeSTR[] = {
			"",
			"PURE",
			"VAR",
			"REG",
			"BIND_NATIVE",
			"BIND_SCRIPT",
			"MAGIC",
			"LOCAL",
			"UPVAL"
		};

		// Operand types refering to a value stored in memory (and not directly in the operand)
		static constexpr bool IsMemoryOperand(OperandType type) {
			return type == OperandType::VAR || type == OperandType::REG || type == OperandType::LOCAL || type == OperandType::UPVAL;
		}

		static inline std::ostream& operator<<(std::ostream& stream, const OperandType& val) {
			stream << OperandTypeSTR[static_cast<std::size_t>(val)];
			return stream;
		}

		struct Operand {
			template<typename T, typename VARIANT_T>
			struct isVariantMember;

			template<typename T, typename... ALL_T>
			struct isVariantMember<T, std::variant<ALL_T...>>
				: public std::disjunction<std::is_same<T, ALL_T>...> {};

			Operand() = default;

			Operand(Operand&&) noexcept = default;
			Operand(const Operand&) = default;
			Operand& operator=(Operand&&) noexcept = default;
			Operand& operator=(const Operand&) = default;

			Operand(const ASTNode& node);

			Operand(OperandVariant var, OperandType type, Cursor position = {}) :
				m_content(std::move(var)),
				m_type(type),
				m_positionInScript(std::move(position)) {
			}

			bool empty() const { return m_type == OperandType::EMPTY; }
			const auto& content() const { return m_content; }
			const auto type() const { return m_type; }
			const auto& position() const { return m_positionInScript; }

			std::string toString() const;

			template <class T>
			T& as() { return std::get<T>(m_content); }

			template <class T>
			const T& as() const { return std::get<T>(m_content); }

			template <class Converted>
			static constexpr bool is_member_of_values() {
				return isVariantMember<Converted, OperandVariant>::value;
			}

		private:
			friend bool operator==(const Operand& lhs, const Operand& rhs);
			friend bool operator!=(const Operand& lhs, const Operand& rhs);

			OperandVariant m_content {};
			OperandType m_type = OperandType::EMPTY;
			Cursor m_positionInScript;
		};

		static inline std::ostream& operator<<(std::ostream& stream, const Operand& val) {
			stream << val.toString();
			return stream;
		}

		bool operator==(const Operand& lhs, const Operand& rhs);
		bool operator!=(const Operand& lhs, const Operand& rhs);

		struct OperandUse : public Operand {
			OperandUse() = default;
			~OperandUse() = default;

			OperandUse& operator=(OperandUse&& op) = default;
			OperandUse& operator=(const OperandUse& op) = default;

			OperandUse(const OperandUse& op) = default;
			OperandUse(OperandUse&& op) = default;


			OperandUse(OperandVariant var, OperandType type, Cursor position = {}) :
				Operand(std::move(var), type, position){
			}
			OperandUse(Operand op) : Operand(std::move(op)) {}

			bool isFirstTimeUsed = false;
		};

		using Register = OperandUse;
	}


}

namespace std {
	template<>
	struct hash<ska::bytecode::Operand> {
		size_t operator()(const ska::bytecode::Operand & x) const {
			const size_t h1 = hash<std::string>()(x.toString());
    		const size_t h2 = hash<std::size_t>()(static_cast<std::size_t>(x.type()));
    		return h1 ^ (h2 << 1);
		}
	};
}
