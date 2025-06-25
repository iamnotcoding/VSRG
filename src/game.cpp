#include <exception>
#include <fstream>
#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <nlohmann/json.hpp>
#include "SDL3/SDL_events.h"
#include "game_scripts/main_script.hpp"

#include "game.hpp"

extern bool is_test_mode_flag;

using json = nlohmann::json;

using namespace game;

template <typename T> static bool set_if_exists(T &dest, const json &data, const std::string &key);
template <typename T> static void set_mandantory(T &dest, const json &data, const std::string &key);

static GameConfig get_game_config(std::string config_file);

static void poll_events();

template <typename T> static bool set_if_exists(T &dest, const json &data, const std::string &key)
{
	if (!data.contains(key))
	{
		return false; // Key does not exist, do not set the value
	}

	dest = data[key].get<T>();
	return true;
}

template <typename T> static void set_mandantory(T &dest, const json &data, const std::string &key)
{
	if (!data.contains(key) || data[key].is_null())
	{
		throw std::runtime_error("'" + key + "' must be specified");
	}
	
	dest = data[key].get<T>();
}

static GameConfig get_game_config(std::string config_file)
{
	GameConfig config;

	std::ifstream f(config_file);

	if (!f.is_open())
	{
		throw std::runtime_error("Failed to open config file");
	}

	json data = json::parse(f);

	set_mandantory(config.title, data, "title");
	set_mandantory(config.width, data, "width");
	set_mandantory(config.height, data, "height");

	set_if_exists(config.is_resizable_flag, data, "is_resizable");
	set_if_exists(config.is_fullscreen_flag, data, "is_fullscreen");
	set_if_exists(config.is_fps_capped_flag, data, "is_fps_capped");

	if (config.is_fps_capped_flag)
	{
		set_mandantory(config.fps, data, "fps");
	}

	return config;
}


namespace game
{
	// Should be called after run_game()
	SDL_Window *window = nullptr; 
	std::vector<SDL_Event> events;

	void update_renderer(SDL_Renderer *renderer)
	{
		if (!renderer)
		{
			throw std::runtime_error("Renderer is not initialized");
		}

		SDL_RenderPresent(renderer);
    	SDL_RenderClear(renderer);
	}

	SDL_Window *get_window()
	{
		return window;
	}

	void run_game(std::string config_file)
	{
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			throw std::runtime_error("Failed to initialize SDL");
		}

		GameConfig config = get_game_config(config_file);
		window = SDL_CreateWindow(config.title.c_str(), config.width, config.height,
											  config.is_resizable_flag ? SDL_WINDOW_RESIZABLE : 0);

		if (!window)
		{
			SDL_Quit();
			throw std::runtime_error("Failed to create SDL window");
		}

		bool running = true;

		main_script::start();

		while (running)
		{
			poll_events();

			for (const auto &event : events)
			{
				if (event.type == SDL_EVENT_QUIT)
				{
					running = false;
				}
			}

			main_script::update();

			if (config.is_fps_capped_flag)
			{
				SDL_Delay(1000 / config.fps);
			}
		}

		SDL_DestroyWindow(window);
		SDL_Quit();
	}
} // namespace game

static void poll_events()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		game::events.push_back(event);
	}
}
