#include "NodeValue.h"
#include "PlainMemoryTable.h"
#include "NodeValueArray.h"

std::ostream& ska::operator<<(std::ostream& stream, const NodeValueArray& nodeArray) {
  for(const auto& val : *nodeArray) {
    stream << val.convertString() << std::endl;
  }
  return stream;
}
