#pragma once

#include <vector>

#include <SDL3/SDL_video.h>
#include <SDL3/sdl.h>

#include "sprite.hpp"

namespace scene
{
	class Scene
	{
	  public:
		SDL_Rect rect;
		
		Scene(int x, int y, int width, int height) : rect{x, y, width, height} {}
		virtual ~Scene() = default;

		void add_sprite(sprite::Sprite *sprite);
		bool remove_sprite(sprite::Sprite *sprite);
		
		void draw();
		virtual void update() = 0;
		void resize(double width_multiplier, double height_multiplier);
		void resize(int width, int height);
		
	  protected:
		bool is_visible_flag = true;
		std::vector<sprite::Sprite *> sprites; // List of sprites in the scene
	};
} // namespace scene