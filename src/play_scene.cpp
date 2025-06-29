#include <iostream>
#include <vector>
#include <chrono>

#include <nlohmann/json.hpp>

#include "bmp_sprite.hpp"
#include "json_util.hpp"
#include "play_scene.hpp"

using namespace play_scene;

using json = nlohmann::json;
using namespace json_util;

PlayScene::PlayScene(std::string map_file, std::string skin_name, int x, int y, int width, int height)
	: scene::Scene(x, y, width, height), map_file(std::move(map_file)), skin_name(std::move(skin_name))
{
	note_bmp_names = new std::string[key_count];

	load_notes(this->map_file);
	load_key_specific_skin_config();
	load_regular_note_bmp_names();

    game_clock = std::chrono::high_resolution_clock::now();
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
}

// nanoseconds
long long PlayScene::get_map_play_duration() 
{
	auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now - game_clock).count();
}

void PlayScene::update()
{

}
