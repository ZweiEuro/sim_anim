
#include "GameCore/GameManager.hpp"
#include "Rendering/Renderer.hpp"
#include "enums.hpp"

#include <stdio.h>
#include <spdlog/spdlog.h>
#include <assert.h>

const float FPS = 60;

int main(int argc, char *argv[])
{
	spdlog::info("Base event id: {}", USER_BASE_EVENT);

	mg8::GameManager::instance()->loop();

	spdlog::info("Exit");
	return 0;
}
