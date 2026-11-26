#include "Serializer/Config/LoggerSerializer.h"
#include "BytecodeCommonSerializer.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::CommonSerializer);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::CommonSerializer)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::CommonSerializer)

void ska::bytecode::CommonSerializer::write(SerializerSafeZone<sizeof(uint32_t)> buffer, std::size_t value) {
	buffer.write(value);
}

void ska::bytecode::CommonSerializer::write(SerializerSafeZone<sizeof(Chunk)> output, std::string value) {
	output.write(std::move(value));
}
