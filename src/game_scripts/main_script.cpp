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

using namespace std;

namespace main_script
{
	unique_ptr<scene::Scene> play_scene;
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

		play_scene = make_unique<scene::Scene>( 0, 0, 400, 400);

		sprite1 = make_shared<bmp_sprite::BmpSprite>(renderer, "../../images.png", 0, 0);
		sprite2 = make_shared<bmp_sprite::BmpSprite>(renderer, "../../images.png", 300, 300);

		play_scene->add_sprite(static_cast<sprite::Sprite *>(sprite1.get()));
		play_scene->add_sprite(static_cast<sprite::Sprite *>(sprite2.get()));
	}

	// A function that's called every frame
	void update()
	{
		play_scene->draw();

		while(!game::events.empty())
		{
			SDL_Event event = game::events.back();
			game::events.pop_back();

			if (event.type == SDL_EVENT_KEY_DOWN)
			{
				switch (event.key.key)
				{
					case SDLK_ESCAPE:
						SDL_Event quit_event;
						quit_event.type = SDL_EVENT_QUIT;
						game::events.push_back(quit_event);
						break;
					case SDLK_B:
						sprite1->scale_self(1.1, 1.1);
						sprite2->scale_self(1.1, 1.1);
						cout << "B key pressed, resizing sprites." << endl;
				}
			}
		}

		game::update_renderer(renderer);
	}
} // namespace main_script