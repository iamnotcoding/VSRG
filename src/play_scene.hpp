#pragma once

#include <chrono>
#include <iostream>
#include <string>


#include "SDL3/SDL_render.h"
#include "game_scripts/map_parser.hpp"
#include <bmp_sprite.hpp>
#include <nlohmann/json.hpp>
#include <prefab.hpp>
#include <scene.hpp>


using json = nlohmann::json;

namespace play_scene
{
	class PlayScene : public scene::Scene
	{
	  public:
		PlayScene(SDL_Renderer *renderer, std::string map_file, std::string skin_name, int x, int y, int width,
				  int height);
		~PlayScene() override;

		void update() override;

	  private:
		SDL_Renderer *renderer; // Renderer for drawing the scene
		std::string map_file;				  // Path to the map file
		std::vector<map_parser::Note> *notes; // List of notes parsed from the map file
		float regular_note_height;
		std::vector<bmp_sprite::BmpSprite *> active_note_sprites;
		prefab::PreFab note_prefab;
		std::string skin_name;
		std::string *note_bmp_names;
		json key_specific_skin_config;
		std::chrono::time_point<std::chrono::high_resolution_clock> game_clock;
		std::chrono::time_point<std::chrono::high_resolution_clock> prev_frame_time;
		// TODO : actually read this
		int key_count = 8; // 7 + 1
		double scroll_speed = 10;

		void load_notes(std::string map_file);
		void load_regular_note_bmp_names();
		void load_key_specific_skin_config();

		long double get_map_play_duration();
		bmp_sprite::BmpSprite *get_new_regular_note_sprite(int lane_index);
	};
} // namespace play_scene