#include <iostream>
#include <stdexcept>
#include <memory>

#include "SDL3/SDL_render.h"
#include "bmp_sprite.hpp"
#include "main_script.hpp"
#include "../game.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include <scene.hpp>
#include "../play_scene.hpp"

using namespace std;

namespace main_script
{
	unique_ptr<play_scene::PlayScene> play_scene;
	SDL_Renderer *renderer;
	shared_ptr<bmp_sprite::BmpSprite> sprite1; 
	shared_ptr<bmp_sprite::BmpSprite> sprite2; 

	// A function that's called once when the game starts.
	void start()
	{
		SDL_Window *window = game::get_window();

		if (!window)
		{
			throw std::runtime_error("Game window is not initialized.");
		}

		renderer = SDL_CreateRenderer(window, nullptr);

		if (!renderer)
		{
			throw std::runtime_error("Failed to create renderer: " + string(SDL_GetError()));
		}

		play_scene = make_unique<play_scene::PlayScene>("ex.bms", "simple", 0, 0, 400, 400);
	}

	// A function that's called every frame
	void update()
	{
		play_scene->update();
		play_scene->draw();
		game::update_renderer(renderer);
	}
} // namespace main_script