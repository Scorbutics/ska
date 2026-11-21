#include "Config/LoggerConfigLang.h"
#include "Runtime/Value/PlainMemoryTable.h"
#include "BytecodeOperand.h"
#include "NodeValue/AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::Operand);

ska::bytecode::Operand::Operand(const ASTNode& node) :
	m_type(OperandType::PURE),
	m_positionInScript(node.positionInScript()) {

	SLOG(LogLevel::Info) << "Converting \"" << node.name() << "\" to \"" << node.type().value() << "\"";

	switch(node.type().value().type()) {
		case ExpressionType::BOOLEAN:
			m_content = node.name() == "true";
			break;
		case ExpressionType::FLOAT:
			m_content = std::stod(node.name());
		break;
		case ExpressionType::INT:
			m_content = std::stol(node.name());
			break;
		case ExpressionType::STRING:
			m_content = std::make_shared<std::string>(node.name());
			break;
		case ExpressionType::VOID:
			m_type = OperandType::EMPTY;
		break;
		default: {
			auto ss = std::stringstream {};
			ss << "unsupported conversion of node type \"" << node.type().value() << "\" for \"" << node.name() << "\"";
			throw std::runtime_error(ss.str());
		}
	}
}

std::string ska::bytecode::Operand::toString() const {
	if(empty()) {
		return "";
	}

	if(std::holds_alternative<StringShared>(m_content)) {
		return *std::get<StringShared>(m_content);
	}

	auto output = std::string {};
	auto script = std::string {};
	std::visit([&output, &script](const auto& operand) {
		using TypeT = std::decay_t<decltype(operand)>;
		if constexpr (std::is_same_v<ScriptVariableRef, TypeT>) {
			output = std::to_string(operand.variable);
			script = std::to_string(operand.script);
		} else if constexpr (!std::is_same_v<StringShared, TypeT>) {
			output = std::to_string(operand);
		}
	}, m_content);


	switch(m_type) {
	case OperandType::VAR:
		output = "V" + output + ":" + script;
		break;

	case OperandType::REG:
		output = "R" + output + ":" + script;
		break;

	case OperandType::LOCAL:
		output = "L" + output + ":" + script;
		break;

	case OperandType::UPVAL:
		output = "U" + output + ":" + script;
		break;

	case OperandType::SELF:
		output = "SELF";
		break;

	default:
		break;
	}

	return output;
}

bool ska::bytecode::operator==(const Operand& lhs, const Operand& rhs) {
	if (std::holds_alternative<StringShared>(lhs.m_content)) {
		if (!std::holds_alternative<StringShared>(rhs.m_content)) return false;

		const auto& leftContent = std::get<StringShared>(lhs.m_content);
		const auto& rightContent = std::get<StringShared>(rhs.m_content);
		if (leftContent == rightContent) return true;
		if (leftContent && rightContent) return *leftContent == *rightContent;
		return false;
	}
	return lhs.m_content == rhs.m_content && lhs.m_type == rhs.m_type;
}

bool ska::bytecode::operator!=(const Operand& lhs, const Operand& rhs) {
	return !operator==(lhs, rhs);
}
