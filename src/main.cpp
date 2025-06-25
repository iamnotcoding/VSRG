#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <iostream>

#include "game.hpp"

using namespace std;

constexpr bool is_test_mode_flag = true;

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
	setlocale(LC_ALL, "UTF-8.en_US");

	game::run_game("../../config.json");

	return 0;
}
