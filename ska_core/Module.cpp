#include <iostream>
#include <string>

#include "Module.h"
#include "Logging/Logger.h"

ska::Module::Module(const std::string& name) :
	m_name(name) {
	SKA_LOG_INFO("Loading module ", m_name);
}

ska::Module::~Module() {
	SKA_LOG_INFO("Unloading module ", m_name);
}
