
/*
 * This program uses the Allegro game library to display a blank window.
 *
 * It initializes the display and starts up the main game loop. The
 * game loop only checks for two events: timer (determined by the FPS)
 * and display close (when the user tries to close the window).
 *
 * http://www.damienradtke.org/building-a-mario-clone-with-allegro
 */
#include "GameManager/GameManager.hpp"
#include "Renderer/Renderer.hpp"
#include "enums.hpp"

#include <stdio.h>
#include <allegro5/allegro.h>
#include <spdlog/spdlog.h>

const float FPS = 60;

int main(int argc, char *argv[])
{
	if (!al_init())
	{
		spdlog::error("Failed to initialize allegro.");
		abort();
	}

	spdlog::info("Base event id: {}", USER_BASE_EVENT);

	mg8::GameManager::instance()->loop();

	spdlog::info("Exit");
	spdlog::shutdown();
	return 0;
}
