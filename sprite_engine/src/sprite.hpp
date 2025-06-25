#pragma once

#include "SDL3/SDL_render.h"
#include <vector>

#include <sdl3/SDL.h>

namespace sprite
{
    class Sprite
    {
      public:
        SDL_Renderer *renderer; // Renderer for drawing the sprite
        SDL_FRect original_rect;  // Changed from SDL_Rect to SDL_FRect
        SDL_FRect rect;           // Changed from SDL_Rect to SDL_FRect

        Sprite(SDL_Renderer *renderer, float x, float y, float width, float height);  // Changed to float parameters
        ~Sprite();

        bool is_visible() const
        {
            return is_visible_flag;
        }

        void show_self()
        {
            is_visible_flag = true;
        }

        void hide_self()
        {
            is_visible_flag = false;
        }

        void show_all();
        void hide_all();

        virtual void draw_self() = 0;
        void draw_all();

        void add_child(Sprite *child);
        bool remove_child(Sprite *child);

        // Changed to float parameters
        virtual void resize_self(float width, float height) = 0;
        void scale_self(double width_multiplier, double height_multiplier);

        void resize_all(double width_multiplier, double height_multiplier);
      protected:
        bool is_visible_flag = true;
        std::vector<Sprite *> children; // List of child sprites
    };   
} // namespace spriteprite
