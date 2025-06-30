#include <stdexcept>
#include <string>

#include "bmp_sprite.hpp"
#include "sprite.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3/sdl.h>
#include <SDL3_image/SDL_image.h>


using namespace bmp_sprite;

// Get texture dimensions before loading fully
std::pair<int, int> BmpSprite::temp_get_texture_size(std::string bmp_name)
{
	// We need a temporary surface to get dimensions
	SDL_Surface *temp_surface = IMG_Load(bmp_name.c_str());

	if (!temp_surface)
	{
		throw std::runtime_error("Failed to load image: " + std::string(SDL_GetError()));
	}

	int width = temp_surface->w;
	int height = temp_surface->h;

	SDL_DestroySurface(temp_surface);

	return {width, height};
}

BmpSprite::BmpSprite(SDL_Renderer *renderer, std::string bmp_name, int x, int y)
	: sprite::Sprite(renderer, x, y, temp_get_texture_size(bmp_name).first, temp_get_texture_size(bmp_name).second)
{
	// Load texture directly from file - much more efficient!
	sprite_texture = IMG_LoadTexture(renderer, bmp_name.c_str());
	original_texture = IMG_LoadTexture(renderer, bmp_name.c_str());

	if (!sprite_texture || !original_texture)
	{
		throw std::runtime_error("Failed to load texture: " + std::string(SDL_GetError()));
	}

	original_rect = rect; // Store the original rectangle for resizing
}

BmpSprite::~BmpSprite()
{
	SDL_DestroyTexture(original_texture);
	SDL_DestroyTexture(sprite_texture);
	// Don't destroy the renderer here!
	// renderer ownership belongs elsewhere
}

void BmpSprite::draw_self()
{
	if (!is_visible_flag)
	{
		return;
	}

	// Render texture (keep your existing check)
	if (!SDL_RenderTexture(renderer, sprite_texture, nullptr, &rect))
	{
		throw std::runtime_error("Failed to render texture: " + std::string(SDL_GetError()));
	}
}

void BmpSprite::resize_self(float width, float height)
{
	if (width <= 0 || height <= 0)
	{
		throw std::invalid_argument("Width and height must be positive");
	}

	// Update the destination rectangle
	rect.w = width;
	rect.h = height;

	// With textures, we don't need to manually rescale!
	// The renderer will handle scaling when rendering the texture
	// This is MUCH faster than SDL_ScaleSurface
}