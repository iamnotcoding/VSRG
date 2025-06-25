#include <iostream>
#include <string>

#include "game_scripts/map_parser.hpp"
#include <scene.hpp>


namespace play_scene
{
	class PlayScene : public scene::Scene
	{
	  public:
		PlayScene(std::string map_file, int x, int y, int width, int height)
			: scene::Scene(x, y, width, height), map_file(std::move(map_file))
		{
			load_notes(map_file);
		}

		~PlayScene()
		{
			delete notes;
		}

	  private:
		std::string map_file;				  // Path to the map file
		std::vector<map_parser::Note> *notes; // List of notes parsed from the map file

		void load_notes(std::string map_file);
	};
} // namespace play_scene