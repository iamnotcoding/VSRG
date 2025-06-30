#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

#include <SDL3/SDL_render.h>
#include <bmp_sprite.hpp>
#include <prefab.hpp>
#include "json_util.hpp"
#include "play_scene.hpp"

using namespace play_scene;

using json = nlohmann::json;
using namespace json_util;

PlayScene::PlayScene(SDL_Renderer *renderer, std::string map_file, std::string skin_name, int x, int y, int width,
					 int height)
	: scene::Scene(x, y, width, height), renderer(renderer), map_file(std::move(map_file)), note_prefab(0, 0),
	  skin_name(std::move(skin_name))
{
	note_bmp_names = new std::string[key_count];

	load_notes(this->map_file);
	load_key_specific_skin_config();
	load_regular_note_bmp_names();

	game_clock = std::chrono::high_resolution_clock::now();
	regular_note_height = rect.h / 10.0f;
}

PlayScene::~PlayScene()
{
	delete notes;
	delete[] note_bmp_names;
}

void PlayScene::load_notes(std::string map_file)
{
	map_parser::Map map;
	map_parser::parse_map(std::ifstream(map_file), map);

	// temporary measure
	notes = new std::vector<map_parser::Note>[key_count];

	for (const auto &note : map.notes)
	{
		notes[note.lane_index].push_back(note);
	}
}

void PlayScene::load_regular_note_bmp_names()
{
	json note_bmp_names_json;

	set_mandatory(note_bmp_names_json, key_specific_skin_config, "regular_notes");

	int bmp_names_count = note_bmp_names_json.size();

	if (bmp_names_count != key_count)
	{
		throw std::runtime_error("Number of regular note BMP names does not match key count: " +
								 std::to_string(bmp_names_count) + " != " + std::to_string(key_count));
	}

	for (int i = 0; i < bmp_names_count; ++i)
	{
		note_bmp_names[i] = note_bmp_names_json[i].get<std::string>();
	}
}

void PlayScene::load_key_specific_skin_config()
{
	std::string config_file_path = "./skins/" + skin_name + "/play/config.json";

	json skin_config = json::parse(std::ifstream(config_file_path));
	json skin_presets;
	json key_specific_preset;
	bool is_key_specific_skin_config_found = false;

	set_mandatory(skin_presets, skin_config, "presets");

	for (const auto &preset : skin_presets.items())
	{
		int cur_key_count;

		set_mandatory(cur_key_count, preset.value(), "key");

		if (cur_key_count == key_count)
		{
			key_specific_skin_config = preset.value();
			is_key_specific_skin_config_found = true;
			break;
		}
	}

	if (!is_key_specific_skin_config_found)
	{
		throw std::runtime_error("No key-specific skin config found for key count: " + std::to_string(key_count));
	}
}

bmp_sprite::BmpSprite *PlayScene::get_new_regular_note_sprite(int lane_index)
{
	int lane_width = rect.w / key_count;

	if (lane_index < 0 || lane_index >= key_count)
	{
		throw std::out_of_range("Lane index out of range: " + std::to_string(lane_index));
	}

	std::string bmp_name = "./skins/" + skin_name + "/play/" + note_bmp_names[lane_index];

	bmp_sprite::BmpSprite *note_sprite =
		new bmp_sprite::BmpSprite(renderer, bmp_name, rect.x + lane_index * lane_width, rect.y - (int)(regular_note_height));

	note_sprite->resize_self(lane_width, (int)(regular_note_height));
	return note_sprite;
}

// milliseconds
long double PlayScene::get_map_play_duration()
{
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(now - game_clock).count() / 1000000.0L;
}

void PlayScene::update()
{
	bool is_all_notes_empty = true;
	double delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() -
																			 prev_frame_time)
							.count() /
						1000000.0; // Convert to milliseconds

	if (delta_time < 0)
	{
		std::cerr << "Negative delta time detected, resetting to 0." << std::endl;
		delta_time = 0;
	}

	for (int i = 0; i < key_count; i++)
	{
		if (notes[i].empty())
		{
			continue; // Skip empty lanes
		}

		is_all_notes_empty = false;				  // At least one lane has notes
		map_parser::Note note = notes[i].front(); // Get the first note in the lane
		// std::cout << "note start time: " << note.start_time << std::endl;
		// std::cout << "map play duration: " << get_map_play_duration() << std::endl;

		/* draw regular notes a bit earlier than they're start time
		so that they can partially be seen on the screen
		otherwise, they will look like they appear out of nowhere */
		if (note.start_time > get_map_play_duration() + 10 * regular_note_height / 10)
		{
			continue;
		}

		std::cout << "new note at lane " << note.lane_index << " with start time " << note.start_time << std::endl;
		bmp_sprite::BmpSprite *note_sprite = get_new_regular_note_sprite(note.lane_index);

		std::cout << "new note sprite addr" << note_sprite << std::endl;

		add_sprite(note_sprite);
		note_prefab.add_child(note_sprite);

		active_note_sprites.push_back(note_sprite);

		notes[i].erase(notes[i].begin());
	}

	note_prefab.move_by_all(0, scroll_speed * delta_time / 10);

	if (is_all_notes_empty)
	{
		std::cout << "All notes are empty, no sprites to draw." << std::endl;
		return; // No notes to draw
	}

	prev_frame_time = std::chrono::high_resolution_clock::now();
}
