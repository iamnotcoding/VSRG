#include <algorithm>
#include <stdexcept>

#include <SDL3/sdl.h>
#include "sprite.hpp"
#include "SDL3/SDL_render.h"

using namespace sprite;

// Updated constructor to use float parameters
Sprite::Sprite(SDL_Renderer *renderer, float x, float y, float width, float height) 
    : renderer(renderer), rect{x, y, width, height}
{
    original_rect = rect; // Store the original rectangle for resizing
}

Sprite::~Sprite()
{
}

void Sprite::draw_all()
{
    if (!is_visible_flag)
    {
        return; // Do not draw if not visible
    }

    draw_self(); // Draw this sprite

    for (auto child : children)
    {
        child->draw_self(); // Draw all child sprites
    }
}

void Sprite::add_child(Sprite *child)
{
    if (child == nullptr)
    {
        throw std::invalid_argument("Child sprite is null");
    }

    children.push_back(child);
}

// Returns true if the child was found and removed, false otherwise
bool Sprite::remove_child(Sprite *child)
{
    if (child == nullptr)
    {
        throw std::invalid_argument("Child sprite is null");
    }

    auto it = std::remove(children.begin(), children.end(), child);

    if (it == children.end())
    {
        return false; // Child not found
    }

    children.erase(it);
    return true; // Child removed successfully
}

void Sprite::show_all()
{
    is_visible_flag = true;

    for (auto child : children)
    {
        child->show_self(); // Show all child sprites
    }
}

void Sprite::hide_all()
{
    is_visible_flag = false;

    for (auto child : children)
    {
        child->hide_self(); // Hide all child sprites
    }
}

void Sprite::scale_self(double width_multiplier, double height_multiplier)
{
    if (width_multiplier <= 0 || height_multiplier <= 0)
    {
        throw std::runtime_error("Width and height multipliers must be positive");
    }

    // Changed to use float calculation
    resize_self(rect.w * static_cast<float>(width_multiplier), 
                rect.h * static_cast<float>(height_multiplier));
}

void Sprite::resize_all(double width_multiplier, double height_multiplier)
{
    if (width_multiplier <= 0 || height_multiplier <= 0)
    {
        throw std::runtime_error("Width and height multipliers must be positive");
    }

    resize_self(width_multiplier, height_multiplier); // Resize this sprite

    for (auto child : children)
    {
        child->resize_self(width_multiplier, height_multiplier); // Resize all child sprites
    }
}