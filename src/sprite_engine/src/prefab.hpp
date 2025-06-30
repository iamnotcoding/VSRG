#pragma once

#include "sprite.hpp"

namespace prefab
{
	class PreFab : public sprite::Sprite
	{
	  public:
		PreFab(int x, int y);
		~PreFab() override = default;
		void draw_self() override;
		void resize_self(float width, float height) override;
	};
} // namespace prefab