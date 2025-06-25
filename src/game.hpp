#pragma once

#include <string>
#include <vector>

#include <SDL3/SDL.h>

namespace game
{
    // This may be modified out of the namesace
    extern std::vector<SDL_Event> events;

    struct GameConfig
    {
        std::string title;
        int width;
        int height;
        bool is_resizable_flag = true;
        bool is_fullscreen_flag = false;
        bool is_fps_capped_flag = false;
        int fps;
    };

	void update_renderer(SDL_Renderer *renderer);
	SDL_Window *get_window();
    void run_game(std::string config_file);
}