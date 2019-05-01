#pragma once

namespace ska {
	
	class StringConstExpr {
	public:
		template <std::size_t N>
		constexpr StringConstExpr(const char(&str)[N]) :
			m_str(str),
			m_length(N - 1) {
		}

		constexpr const char* c_str() const {
			return m_str;
		}

		constexpr std::size_t size() const { 
			return m_length; 
		}
		
		constexpr char operator[](const std::size_t i) const {
			return m_str[i];
		}

		constexpr bool operator==(const StringConstExpr& sce) const {
			auto i = 0u;
			for (; i < m_length && i < sce.m_length && m_str[i] == sce[i]; i++);
			return i == m_length && i == sce.m_length;
		}

	private:
		const char* m_str;
		const std::size_t m_length;
	};
}