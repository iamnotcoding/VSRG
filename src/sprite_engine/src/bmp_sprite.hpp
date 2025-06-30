#pragma once

#include <string>
#include <utility>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include "sprite.hpp"

namespace bmp_sprite
{
    class BmpSprite : public sprite::Sprite
    {
      private:
        SDL_Texture *sprite_texture = nullptr;
        SDL_Texture *original_texture = nullptr;

        static std::pair<int,int> temp_get_texture_size(std::string bmp_name);

      public:
        BmpSprite(SDL_Renderer *renderer, std::string bmp_name, int x, int y);
        ~BmpSprite();

        void draw_self() override;
        void resize_self(float width, float height) override;
		using sprite::Sprite::resize_self; // Use the base class resize_self for multipliers
    };
} // namespace bmp_sprite
