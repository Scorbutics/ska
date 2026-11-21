#include <sstream>
#include "Config/LoggerConfigLang.h"
#include <array>
#include "Runtime/Value/PlainMemoryTable.h"
#include "NodeValue/AST.h"
#include "BytecodeTypeConversion.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "NodeValue/Type.h"

namespace ska {
	namespace bytecode {

		template <Command cmd1, Command cmd2>
		struct CommandPackOr { static constexpr auto first = cmd1; static constexpr auto second = cmd2; };

		struct TypeConversionData {
			Command command;
			Command commandReverse;
		};

		/*
		using TypeToInstructionConverterCallback = std::function<Command(bool)>;
		using TypeToInstructionConverter = std::variant<Command, TypeToInstructionConverterCallback>;
		*/
		using TypeToInstructionConverter = OperationList<TypeConversionData>;

		using TypeToInstructionConverterContainer = std::array<TypeToInstructionConverter, static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length)>;
		using TwoTypesConverterContainer = std::array<TypeToInstructionConverterContainer, static_cast<std::size_t>(ExpressionType::UNUSED_Last_Length)>;
		using TypeConvertToInstructionContainer = std::array<TwoTypesConverterContainer, static_cast<std::size_t>(LogicalOperator::UNUSED_Last_Length)>;

		template <Command cmd>
		struct CommandComplex : std::false_type {};
		template <> struct CommandComplex<Command::CONV_I_D> : std::true_type {};
		template <> struct CommandComplex<Command::CONV_D_I> : std::true_type {};
		template <> struct CommandComplex<Command::CONV_I_STR> : std::true_type {};
		template <> struct CommandComplex<Command::CONV_D_STR> : std::true_type {};

		static void EnqueueConverterItemFunc(TypeConvertToInstructionContainer& container, LogicalOperator op, ExpressionType t1, ExpressionType t2, TypeToInstructionConverter value) {
			container[static_cast<std::size_t>(op)][static_cast<std::size_t>(t1)][static_cast<std::size_t>(t2)] = std::move(value);
		}

		template <class Item>
		struct BuildCommandPack {
			static TypeConversionData build() {
				return { Item::first, Item::second };
			}
		};

		template <Command item>
		struct BuildCommand {
			static TypeConversionData build() {
				return { item, item };
			}
		};

		template <OperationType operation, Command ... command>
		struct EnqueueConverterItem {
			static_assert(sizeof...(command) <= 2, "Unsupported complex command list (size > 2)");
			static void enqueue(TypeConvertToInstructionContainer& container, LogicalOperator op, ExpressionType t1, ExpressionType t2) {
				EnqueueConverterItemFunc(container, op, t1, t2, TypeToInstructionConverter{ operation, { BuildCommand<command>::build()... } });
			}
		};

		template <OperationType operation, class ... command>
		struct EnqueueConverterItemPack {
			static_assert(sizeof...(command) <= 2, "Unsupported complex command list (size > 2)");
			static void enqueue(TypeConvertToInstructionContainer& container, LogicalOperator op, ExpressionType t1, ExpressionType t2) {
				EnqueueConverterItemFunc(container, op, t1, t2, TypeToInstructionConverter{ operation, { BuildCommandPack<command>::build()... } });
			}
		};

		static TypeConvertToInstructionContainer BuildTypeConverter() {
			auto result = TypeConvertToInstructionContainer{};

			// Direct types (no conversion) (full first)
			// Maths
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::ADD_I>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::ADD_D>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::FLOAT, ExpressionType::FLOAT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::ADD_STR>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::STRING, ExpressionType::STRING);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::PUSH_ARR_ARR>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::ARRAY, ExpressionType::ARRAY);

			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_I>::enqueue(result, LogicalOperator::SUBSTRACT, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_D>::enqueue(result, LogicalOperator::SUBSTRACT, ExpressionType::FLOAT, ExpressionType::FLOAT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_ARR>::enqueue(result, LogicalOperator::SUBSTRACT, ExpressionType::ARRAY, ExpressionType::ARRAY);

			EnqueueConverterItem<OperationType::FULL_FIRST, Command::MUL_I>::enqueue(result, LogicalOperator::MULTIPLY, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::MUL_D>::enqueue(result, LogicalOperator::MULTIPLY, ExpressionType::FLOAT, ExpressionType::FLOAT);

			EnqueueConverterItem<OperationType::FULL_FIRST, Command::DIV_I>::enqueue(result, LogicalOperator::DIVIDE, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::DIV_D>::enqueue(result, LogicalOperator::DIVIDE, ExpressionType::FLOAT, ExpressionType::FLOAT);

			// Logic
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_I, Command::TEST_EQ>::enqueue(result, LogicalOperator::EQUALITY, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_D, Command::TEST_EQ>::enqueue(result, LogicalOperator::EQUALITY, ExpressionType::FLOAT, ExpressionType::FLOAT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CMP_STR, Command::TEST_EQ>::enqueue(result, LogicalOperator::EQUALITY, ExpressionType::STRING, ExpressionType::STRING);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CMP_ARR, Command::TEST_EQ>::enqueue(result, LogicalOperator::EQUALITY, ExpressionType::ARRAY, ExpressionType::ARRAY);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CMP_BOOL, Command::TEST_EQ>::enqueue(result, LogicalOperator::EQUALITY, ExpressionType::BOOLEAN, ExpressionType::BOOLEAN);

			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CONV_D_I>::enqueue(result, LogicalOperator::EQUAL, ExpressionType::FLOAT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CONV_I_D>::enqueue(result, LogicalOperator::EQUAL, ExpressionType::INT, ExpressionType::FLOAT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CONV_I_STR>::enqueue(result, LogicalOperator::EQUAL, ExpressionType::INT, ExpressionType::STRING);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CONV_STR_I>::enqueue(result, LogicalOperator::EQUAL, ExpressionType::STRING, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CONV_D_STR>::enqueue(result, LogicalOperator::EQUAL, ExpressionType::FLOAT, ExpressionType::STRING);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CONV_STR_D>::enqueue(result, LogicalOperator::EQUAL, ExpressionType::STRING, ExpressionType::FLOAT);

			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_I, Command::TEST_NEQ>::enqueue(result, LogicalOperator::INEQUALITY, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_D, Command::TEST_NEQ>::enqueue(result, LogicalOperator::INEQUALITY, ExpressionType::FLOAT, ExpressionType::FLOAT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CMP_STR, Command::TEST_NEQ>::enqueue(result, LogicalOperator::INEQUALITY, ExpressionType::STRING, ExpressionType::STRING);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CMP_ARR, Command::TEST_NEQ>::enqueue(result, LogicalOperator::INEQUALITY, ExpressionType::ARRAY, ExpressionType::ARRAY);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::CMP_BOOL, Command::TEST_NEQ>::enqueue(result, LogicalOperator::INEQUALITY, ExpressionType::BOOLEAN, ExpressionType::BOOLEAN);

			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_I, Command::TEST_G>::enqueue(result, LogicalOperator::GREATER, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_D, Command::TEST_G>::enqueue(result, LogicalOperator::GREATER, ExpressionType::FLOAT, ExpressionType::FLOAT);

			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_I, Command::TEST_GE>::enqueue(result, LogicalOperator::GREATER_OR_EQUAL, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_D, Command::TEST_GE>::enqueue(result, LogicalOperator::GREATER_OR_EQUAL, ExpressionType::FLOAT, ExpressionType::FLOAT);

			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_I, Command::TEST_L>::enqueue(result, LogicalOperator::LESSER, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_D, Command::TEST_L>::enqueue(result, LogicalOperator::LESSER, ExpressionType::FLOAT, ExpressionType::FLOAT);

			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_I, Command::TEST_LE>::enqueue(result, LogicalOperator::LESSER_OR_EQUAL, ExpressionType::INT, ExpressionType::INT);
			EnqueueConverterItem<OperationType::FULL_FIRST, Command::SUB_D, Command::TEST_LE>::enqueue(result, LogicalOperator::LESSER_OR_EQUAL, ExpressionType::FLOAT, ExpressionType::FLOAT);

			// Conversions (split except for arrays)
			EnqueueConverterItem<OperationType::SPLIT, Command::CONV_I_STR, Command::ADD_STR>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::STRING);
			EnqueueConverterItem<OperationType::SPLIT, Command::CONV_D_STR, Command::ADD_STR>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::FLOAT, ExpressionType::STRING);
			EnqueueConverterItem<OperationType::SPLIT, Command::CONV_I_D, Command::ADD_D>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::FLOAT);
			EnqueueConverterItemPack<OperationType::FULL_FIRST, CommandPackOr<Command::PUSH_F_ARR, Command::PUSH_B_ARR>>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::INT, ExpressionType::ARRAY);
			EnqueueConverterItemPack<OperationType::FULL_FIRST, CommandPackOr<Command::PUSH_F_ARR, Command::PUSH_B_ARR>>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::STRING, ExpressionType::ARRAY);
			EnqueueConverterItemPack<OperationType::FULL_FIRST, CommandPackOr<Command::PUSH_F_ARR, Command::PUSH_B_ARR>>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::FLOAT, ExpressionType::ARRAY);
			EnqueueConverterItemPack<OperationType::FULL_FIRST, CommandPackOr<Command::PUSH_F_ARR, Command::PUSH_B_ARR>>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::BOOLEAN, ExpressionType::ARRAY);
			EnqueueConverterItemPack<OperationType::FULL_FIRST, CommandPackOr<Command::PUSH_F_ARR, Command::PUSH_B_ARR>>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::FUNCTION, ExpressionType::ARRAY);
			EnqueueConverterItemPack<OperationType::FULL_FIRST, CommandPackOr<Command::PUSH_F_ARR, Command::PUSH_B_ARR>>::enqueue(result, LogicalOperator::ADDITION, ExpressionType::OBJECT, ExpressionType::ARRAY);

			EnqueueConverterItem<OperationType::SPLIT, Command::CONV_I_D, Command::SUB_D>::enqueue(result, LogicalOperator::SUBSTRACT, ExpressionType::INT, ExpressionType::FLOAT);
			EnqueueConverterItemPack<OperationType::FULL_FIRST, CommandPackOr<Command::POP_F_ARR, Command::POP_B_ARR>>::enqueue(result, LogicalOperator::SUBSTRACT, ExpressionType::INT, ExpressionType::ARRAY);

			EnqueueConverterItem<OperationType::SPLIT, Command::CONV_I_D, Command::MUL_D>::enqueue(result, LogicalOperator::MULTIPLY, ExpressionType::INT, ExpressionType::FLOAT);
			EnqueueConverterItemPack<OperationType::SPLIT, CommandPackOr<Command::MUL_I_ARR, Command::MUL_ARR_I>>::enqueue(result, LogicalOperator::MULTIPLY, ExpressionType::INT, ExpressionType::ARRAY);
			EnqueueConverterItemPack<OperationType::SPLIT, CommandPackOr<Command::MUL_I_STR, Command::MUL_STR_I>>::enqueue(result, LogicalOperator::MULTIPLY, ExpressionType::INT, ExpressionType::STRING);

			EnqueueConverterItem<OperationType::SPLIT, Command::CONV_I_D, Command::DIV_D>::enqueue(result, LogicalOperator::DIVIDE, ExpressionType::INT, ExpressionType::FLOAT);

			return result;
		}

		static const TypedOperandRef& ConvertWhich(const Type& destinationType, const TypedOperandRef& node1, const TypedOperandRef& node2) {
			if(destinationType != ExpressionType::BOOLEAN && node1.type != destinationType && node2.type != destinationType) {
				throw std::runtime_error("unable to perform node conversion");
			}

			if(node1.type == node2.type) {
				return node1;
			}
			return node1.type == destinationType ? node2 : node1;
		}

		CommandList TypeConversion(LogicalOperator logicalOperator, const Type& operandType, const Type& operand2Type, bool reverseOrder) {
			static TypeConvertToInstructionContainer typeConverter = BuildTypeConverter();

			const auto operatorIndex = static_cast<std::size_t>(logicalOperator);
			const auto nodeExpressionTypeIndex = static_cast<std::size_t>(operandType.type());
			const auto resultExpressionTypeIndex = static_cast<std::size_t>(operand2Type.type());

			const auto& converterCmd = typeConverter[operatorIndex][nodeExpressionTypeIndex][resultExpressionTypeIndex];
			auto result = CommandList{ };
			result.type = converterCmd.type;
			if(reverseOrder) {
				for(const auto& cmd : converterCmd.container) {
					result.container.push_back(cmd.commandReverse);
				}
			} else {
				for(const auto& cmd : converterCmd.container) {
					result.container.push_back(cmd.command);
				}
			}
			return result;
		}
	}
}

SKA_LOGC_CONFIG(ska::LogLevel::Error, ska::bytecode::TypeConversionData);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::TypeConversionData)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::TypeConversionData)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::bytecode::TypeConversionData)

ska::bytecode::InstructionOutput ska::bytecode::TypeConversionBinary(LogicalOperator logicalOperator, const TypedOperandRef& node1, const TypedOperandRef& node2, const TypedOperandRef& destination) {
	LOG_DEBUG << "Binary operation for nodes " << node1.operand.toString() << " and " << node2.operand.toString() << " with types " << node1.type  << " and " << node2.type;
	const auto& selectedNode = ConvertWhich(destination.type, node1, node2);
	const auto reverseOrder = &selectedNode == &node2;
	const auto& unselectedNode = reverseOrder ? node1 : node2;

	auto result = TypeConversion(logicalOperator, selectedNode.type, unselectedNode.type, reverseOrder);
	if(result.container.empty()) {
		auto ss = std::stringstream{};
		ss << "unsupported operation between the types \"" << node1.type << "\" and \"" << node2.type << "\"";
		LOG_ERROR << ss.str();
		throw std::runtime_error(ss.str());
	}

	assert(result.container.size() == 2 || result.container.size() == 1);

	auto& container = result.container;
	switch (result.type) {
		case OperationType::SPLIT: {
			auto group = InstructionOutput{ Instruction {container[0], destination.operand, selectedNode.operand} };
			group.push(Instruction{ container[1], destination.operand, reverseOrder ? node1.operand : destination.operand, reverseOrder ? destination.operand : node2.operand });
			LOG_INFO << "Conversion detected : " << group;
			return group;
		}
		case OperationType::FULL_FIRST:
		default: {
			//LOG_DEBUG << "Command output : " << CommandSTR[static_cast<std::size_t>(container[0])];
			auto group = InstructionOutput{ Instruction{ container[0], destination.operand, node1.operand, node2.operand } };
			if (container.size() > 1) {
				group.push(Instruction{ container[1], destination.operand, destination.operand });
			}
			LOG_INFO << "Conversion detected : " << group;
			return group;
		}
	}

}

