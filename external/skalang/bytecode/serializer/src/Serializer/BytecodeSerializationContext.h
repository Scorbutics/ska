#pragma once
#include <sstream>
#include <memory>
#include <deque>
#include <vector>
#include <cstdint>
#include <unordered_set>
#include "NodeValue/ASTNodePtr.h"

#include "BytecodeSerializationStrategy.h"
#include "BytecodeChunk.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "Serializer/Symbol/BytecodeSymbolTableSerializer.h"

namespace ska {
	namespace bytecode {

		struct SerializationContext {
			using NativesContainer = std::unordered_map<std::string, std::size_t>;
			SerializationContext(ScriptCache& cache, SerializationStrategy strategy);

			bool next(std::deque<std::size_t> partIndexes);

			bool currentScriptBridged() const { return m_cache[m_id].program().isBridged(); }

			std::size_t writeHeader(std::size_t serializerVersion);
			std::pair<std::size_t, std::vector<std::string>> writeInstructions();
			std::size_t writeSymbolTable();
			std::size_t writeExternalReferences(std::vector<std::string> linkedScripts);

		private:
			void commit(std::deque<std::size_t> partIndexes);
			void push();
			auto& buffer() { return m_buffer.back(); }
			std::size_t instructionsSize() { return m_cache[m_id].size(); }
			const std::string& currentScriptName() const { return m_cache[m_id].name(); }
			
			const std::string scriptName(std::size_t id) const {
				if (!m_cache.exist(id)) { throw std::runtime_error("script with id \"" + std::to_string(id) + "\" does not exist"); }
				return m_cache[id].name();
			}

			std::size_t currentScriptId() const { return m_id; }
			
			std::size_t pushNatives();
			auto begin() const { return m_cache[m_id].begin(); }
			auto end() const { return m_cache[m_id].end(); }

			void operator<<(const Instruction& value);
			void operator<<(const Operand& value);

			const ScriptCache& m_cache;
			std::size_t m_id = 0;
			SerializationStrategy m_strategy;
			order_indexed_string_map<std::string> m_natives;
			std::vector<std::stringstream> m_buffer;
			std::ostream* m_output = nullptr;
			SymbolTableSerializer m_symbolsSerializer;
		};

	}
}
