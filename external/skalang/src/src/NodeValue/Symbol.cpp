#include "Config/LoggerConfigLang.h"
#include "NodeValue/Symbol.h"
#include "Service/ScopedSymbolTable.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Symbol)

#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::Symbol)

ska::Symbol::Symbol(std::size_t tableIndex, std::string name, bool exported) :
	m_name(std::move(name)),
	m_tableIndex(tableIndex),
	m_exported(exported) {
}

ska::Symbol::Symbol(Symbol&& s) noexcept {
	*this = std::move(s);
}

ska::Symbol& ska::Symbol::operator=(Symbol&& s) noexcept {
	m_name = std::move(s.m_name);
	m_tableIndex = std::move(s.m_tableIndex);
	m_category = std::move(s.m_category);
	m_exported = std::move(s.m_exported);
	SLOG(ska::LogLevel::Debug) << "   Move, Symbol " << s.name() << " " << s.m_category << " moved to " << m_name << " " << m_category;
	return *this;
}

bool ska::Symbol::operator==(const Symbol& sym) const {
	return m_name == sym.m_name &&
		m_tableIndex == sym.m_tableIndex;
}

bool ska::Symbol::changeTypeIfRequired(const Type& type) {
	if (m_category == ExpressionType::VOID || m_category.type() == type.type()) {
		m_category = type;
		return true;
	}

	return m_category.tryChangeSymbol(type);
}

std::ostream& ska::operator<<(std::ostream& stream, const Symbol& symbol) {
	stream << symbol.m_name << " (" << symbol.m_category << ")";
	return stream;
}
