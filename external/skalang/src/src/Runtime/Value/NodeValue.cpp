#include <charconv>
#include <iterator>
#include "Config/LoggerConfigLang.h"
#include <cassert>
#include "Runtime/Value/PlainMemoryTable.h"
#include "Runtime/Value/TokenVariant.h"
#include "NodeValue.h"
#include "StringShared.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::NodeValue);

double ska::NodeValue::convertNumeric() const {
	double numeric = 0.0;
	if (m_cell != nullptr) {
		return m_cell->convertNumeric();
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

ska::NodeValue& ska::NodeValue::operator=(const NodeValue& arg) {
	if (this == &arg) {
		return *this;
	}
	if (m_cell != nullptr) {
		*m_cell = arg.resolved();
		return *this;
	}
	m_variant = arg.m_variant;
	m_emptyVariant = arg.m_emptyVariant;
	m_cell = arg.m_cell;
	m_upvalues = arg.m_upvalues;
	return *this;
}

ska::NodeValue& ska::NodeValue::operator=(NodeValue&& arg) noexcept {
	if (this == &arg) {
		return *this;
	}
	if (m_cell != nullptr) {
		*m_cell = std::move(arg).resolved();
		return *this;
	}
	m_variant = std::move(arg.m_variant);
	m_emptyVariant = arg.m_emptyVariant;
	m_cell = std::move(arg.m_cell);
	m_upvalues = std::move(arg.m_upvalues);
	return *this;
}

ska::NodeValue ska::NodeValue::CellReference(Cell cell) {
	assert(cell != nullptr && !cell->isCellReference() && "a cell must contain a value, not a cell reference");
	auto result = NodeValue{};
	result.m_cell = std::move(cell);
	result.m_emptyVariant = false;
	return result;
}

const ska::NodeValue::Upvalues* ska::NodeValue::upvalues() const {
	if (m_cell != nullptr) {
		return m_cell->upvalues();
	}
	return m_upvalues.get();
}

ska::NodeValue ska::NodeValue::resolved() && {
	if (m_cell == nullptr) {
		return std::move(*this);
	}
	return static_cast<const NodeValue&>(*m_cell);
}

ska::NodeValue ska::NodeValue::resolved() const & {
	if (m_cell == nullptr) {
		return *this;
	}
	return static_cast<const NodeValue&>(*m_cell);
}

std::string ska::NodeValue::FormatNumber(double value) {
	// Shortest text that reads back the same number (1.5, 0.1, 3)
	char buffer[64];
	const auto [end, error] = std::to_chars(std::begin(buffer), std::end(buffer), value);
	if (error != std::errc{}) {
		return std::to_string(value);
	}
	return std::string(buffer, end);
}

std::string ska::NodeValue::convertString() const {
	auto result = std::string{};
	if(empty()) {
		return result;
	}

	if (m_cell != nullptr) {
		return m_cell->convertString();
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
				result = FormatNumber(arg);
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

namespace ska {
	bool operator==(const NodeValue& lhs, const NodeValue& rhs) {
		if (lhs.m_cell != nullptr || rhs.m_cell != nullptr) {
			return lhs.resolved() == rhs.resolved();
		}
		return lhs.m_variant == rhs.m_variant;
	}

	std::ostream& operator<<(std::ostream& stream, const NodeValue& value) {
		stream << value.convertString();
		return stream;
	}
}
