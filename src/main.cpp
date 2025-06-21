#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <iostream>

#include "map_parser.hpp"

using namespace std;

constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 1000;

constexpr bool is_test = true;

void update(SDL_Window *window);
void test();

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
	setlocale(LC_ALL, "UTF-8.en_US");

	if (is_test)
	{
		test();
		return 0;
	}

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("vsrg", DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_RESIZABLE);

	if (!window)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	bool running = true;
	SDL_Event event;

	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				running = false;
			}
		}

		SDL_Delay(16); // ~60 FPS
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void test()
{
	cout << "Testing map parser..." << endl;
	map_parser::test();
}

// This funtion is called for every frame
void update(SDL_Window *window)
{
}