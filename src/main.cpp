
/*
 * This program uses the Allegro game library to display a blank window.
 *
 * It initializes the display and starts up the main game loop. The
 * game loop only checks for two events: timer (determined by the FPS)
 * and display close (when the user tries to close the window).
 *
 * http://www.damienradtke.org/building-a-mario-clone-with-allegro
 */
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
