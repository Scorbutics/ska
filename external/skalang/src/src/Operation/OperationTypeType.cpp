#include "OperationTypeType.h"
#include "Service/SymbolTable.h"
#include "NodeValue/ExpressionType.h"

const ska::ScopedSymbolTable* ska::OperationType<ska::Operator::TYPE>::GetTypeSymbol(const SymbolTable& symbolTable) const {
	assert(node.size() > 0);
	auto& typeNameNode = node[0];
	auto* type = symbolTable[typeNameNode.name()];
	if (type != nullptr && typeNameNode.size() >= 1) {
		const std::string& field = typeNameNode[0].name();
		return (*type)[field];
	}
	return type;
}

const ska::Type& ska::OperationType<ska::Operator::TYPE>::GetType() const {
	assert(node.size() > 0);
	return node[0].type().value();
}

bool ska::OperationType<ska::Operator::TYPE>::IsBuiltIn() const {
	return node.size() > 0 && node[0].size() == 0 && ExpressionTypeMap.find(node[0].name()) != ExpressionTypeMap.end();
}

bool ska::OperationType<ska::Operator::TYPE>::IsObject() const {
	return node.size() > 1 && !node[1].logicalEmpty() && !IsBuiltIn();
}

std::string ska::OperationType<ska::Operator::TYPE>::GetName() const {
	assert(node.size() > 0);
	auto& typeNode = node[0];
	return typeNode.size() == 1 ? (typeNode.name() + "::" + typeNode[0].name()) : typeNode.name();
}

std::string ska::OperationType<ska::Operator::TYPE>::GetTypeName() const {
	assert(node.size() > 0);
	auto& typeNode = node[0];
	auto ss = std::stringstream{};
	ss << typeNode.type().value();
	return ss.str();
}
