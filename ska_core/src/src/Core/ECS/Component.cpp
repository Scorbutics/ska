#include "Component.h"

std::string ska::ComponentNameExtract(const char* componentName) {	
	/*
	const auto componentNameSize = strlen(componentName);
	if (componentNameSize == 0) {
		return "";
	}

	int index;
	for (index = componentNameSize; index >= 0 && componentName[index] != ':'; index--);
	index = (index < 0 ? -1 : index) + 1;
	return componentName + index;
	*/
	return componentName;
}
