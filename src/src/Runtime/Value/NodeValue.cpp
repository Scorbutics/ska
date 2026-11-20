#include "Config/LoggerConfigLang.h"
#include "Runtime/Value/PlainMemoryTable.h"
#include "Runtime/Value/TokenVariant.h"
#include "NodeValue.h"
#include "StringShared.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::NodeValue);

double ska::NodeValue::convertNumeric() const {
	double numeric = 0.0;
	if(std::holds_alternative<NodeValue*>(m_variant)) {
		return std::get<NodeValue*>(m_variant)->convertNumeric();
	}
	const auto& valueVariant = std::get<TokenVariant>(m_variant);
	std::visit([&numeric](auto && arg){
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same<T, long>::value) {
			numeric = static_cast<double>(arg);
		} else if constexpr (std::is_same<T, int>::value) {
			numeric = arg;
		} else if constexpr (std::is_same<T, double>::value) {
			numeric = arg;
		} else if constexpr (std::is_same<T, bool>::value) {
			numeric = arg ? 1.0 : 0.0;
		} else if constexpr (std::is_same<T, VariableRef>::value) {
			numeric = static_cast<double>(arg.variable);
		} else if constexpr (std::is_same<T, ScriptVariableRef>::value) {
			numeric = static_cast<double>(arg.variable);
		} else if constexpr (std::is_same<T, StringShared>::value) {
			try {
				numeric = std::stod(*arg);
			} catch(std::logic_error& e) {
				auto ss = std::stringstream{};
				ss << "cannot convert the node value \"" << *arg << "\" to a numeric format : " << e.what();
				throw std::runtime_error(ss.str());
			}
		} else {
			throw std::runtime_error("cannot convert the node value to a numeric format");
		}
	}, valueVariant);
	return numeric;
}

ska::NodeValue ska::NodeValue::resolved() && {
	if (!isReference(m_variant)) {
		return std::move(*this);
	}
	return static_cast<const NodeValue&>(*this).resolved();
}

ska::NodeValue ska::NodeValue::resolved() const & {
	if (!isReference(m_variant)) {
		return *this;
	}
	auto copy = NodeValue { dereference(m_variant) };
	if (!copy.hasEnv()) {
		copy.captureEnvironment = captureEnvironment;
	}
	return copy;
}

void ska::NodeValue::release() {
	m_dirty = true;
}

std::string ska::NodeValue::convertString() const {
	auto result = std::string{};
	if(empty()) {
		return result;
	}

	if(std::holds_alternative<NodeValue*>(m_variant)) {
		return std::get<NodeValue*>(m_variant)->convertString();
	}
	if(std::holds_alternative<TokenVariant>(m_variant)) {
		const auto& valueVariant = std::get<TokenVariant>(m_variant);
		std::visit([&result](auto && arg) {
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same<T, long>::value) {
				result = std::to_string(arg);
			} else if constexpr (std::is_same<T, int>::value) {
				result = std::to_string(arg);
			} else if constexpr (std::is_same<T, double>::value) {
				result = std::to_string(arg);
			} else if constexpr (std::is_same<T, VariableRef>::value) {
				result = std::to_string(arg.variable);
			} else if constexpr (std::is_same<T, ScriptVariableRef>::value) {
				result = "__script " + std::to_string(arg.script) + ": variable ref " + std::to_string(arg.variable) + "__";
			} else if constexpr (std::is_same<T, bool>::value) {
				result = arg ? "true" : "false";
			} else if constexpr (std::is_same<T, StringShared>::value) {
				result = arg ? *arg : "";
			} else if constexpr (std::is_same_v<T, ObjectMemory> || std::is_same_v<T, NativeFunctionPtr>) {
				result = "__complex memory object__";
			} else {
				throw std::runtime_error("cannot convert the node value to a string format");
			}
		}, valueVariant);
	} else {
		result = "__container__";
	}
	return result;
}

const ska::NodeValue* ska::NodeValue::resolveFromEnv(ScriptVariableRef variable) const {
	return captureEnvironment == nullptr || captureEnvironment->find(variable) == captureEnvironment->end() ? nullptr : &captureEnvironment->at(variable);
}

ska::NodeValue* ska::NodeValue::resolveFromEnv(ScriptVariableRef variable) {
	return captureEnvironment == nullptr || captureEnvironment->find(variable) == captureEnvironment->end() ? nullptr : &captureEnvironment->at(variable);
}

std::string ska::NodeValue::printEnv() const {
	auto ss = std::stringstream {};
	if (captureEnvironment) {
		for (const auto& scriptMemory: (*captureEnvironment)) {
			ss << scriptMemory.second;
		}
	}
	return ss.str();
}

void ska::NodeValue::captureInEnv(ScriptVariableRef variable, NodeValue value) {
	if (captureEnvironment == nullptr) {
		captureEnvironment = std::make_shared<std::remove_reference_t<decltype(*captureEnvironment)>>();
	}
	(*captureEnvironment)[variable] = std::move(value);
}

void ska::NodeValue::overrideInEnv(ScriptVariableRef index, NodeValue value) {
	if (captureEnvironment == nullptr || captureEnvironment->find(index) == captureEnvironment->end()) {
		return;
	}
	(*captureEnvironment)[index] = std::move(value);
}

void ska::NodeValue::stealEnv(NodeValue& value) {
	captureEnvironment = std::move(value.captureEnvironment);
	value.captureEnvironment = nullptr;
}

void ska::NodeValue::stealEnv(NodeValue&& value) {
	captureEnvironment = std::move(value.captureEnvironment);
}

ska::NodeValue& ska::NodeValue::dereference(const NodeValueVariant_& variant) {
	auto& referedObject = *std::get<NodeValue*>(variant);
	if (isReference(referedObject.m_variant)) {
		return dereference(referedObject.m_variant);
	}
	return referedObject;
}

void ska::NodeValue::transferValueToOwned(NodeValueVariant_ arg) {
	if (m_dirty) {
		if (isReference(arg)) {
			m_variant = dereference(arg).m_variant;
		} else {
			m_variant = std::move(arg);
		}
		m_dirty = false;
		return;
	}

	if(isReference(m_variant)) {
		SLOG(LogLevel::Debug) << "%11cReference detected, reassigning refered value";
		if (isReference(arg)) {
			dereference(m_variant) = dereference(arg);
		} else {
			dereference(m_variant) = NodeValue { arg };
		}
	} else {
		if(isReference(arg) && std::get<NodeValue*>(arg) == this) {
			SLOG(LogLevel::Error) << "a value ref cannot refer to itself. Aborting assignation.";
			return;
		}
		m_variant = std::move(arg);

		SLOG(LogLevel::Debug) << "%10cAssigning direct value \"" << convertString() << "\"";
		if (captureEnvironment) {
			SLOG(LogLevel::Debug) << "  with capture env ";
			for(const auto& scriptMemory: *captureEnvironment) {
				SLOG(LogLevel::Debug) << scriptMemory.second;
			}
		}
	}
}

bool ska::NodeValue::isReference(const NodeValueVariant_& arg) {
	return std::holds_alternative<NodeValue*>(arg) && std::get<NodeValue*>(arg) != nullptr;
}

namespace ska {
	bool operator==(const NodeValue& lhs, const NodeValue& rhs) {
		return lhs.m_variant == rhs.m_variant;
	}

	std::ostream& operator<<(std::ostream& stream, const NodeValue& value) {
		stream << value.convertString();
		return stream;
	}
}
