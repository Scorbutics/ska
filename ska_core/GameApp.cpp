#include "GameApp.h"

ska::GameApp::GameApp(ska::GameConfiguration&& gc) :
	m_gameConfig(std::forward<ska::GameConfiguration>(gc)) {
}