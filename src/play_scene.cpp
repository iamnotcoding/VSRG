#include "play_scene.hpp"

using namespace play_scene;

void PlayScene::load_notes(std::string map_file)
{
    map_parser::Map map;
    map_parser::parse_map(std::ifstream(map_file), map);

    notes = new vector<map_parser::Note>[8];
    for (const auto &note : map.notes)
    {

    }
}