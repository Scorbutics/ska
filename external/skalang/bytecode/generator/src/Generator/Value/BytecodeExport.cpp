#include "BytecodeExport.h"


bool ska::bytecode::operator==(const ExportSymbol& lhs, const ExportSymbol& rhs) {
  return lhs.value == rhs.value;
}

bool ska::bytecode::operator!=(const ExportSymbol& lhs, const ExportSymbol& rhs) {
  return lhs.value != rhs.value;
}