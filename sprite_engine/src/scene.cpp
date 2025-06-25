#include "sprite.hpp"
#include "scene.hpp"

#include <algorithm>

#include <SDL3/SDL_video.h>
#include <stdexcept>

using namespace scene;
using namespace sprite;

// All sprites including children should be added
void Scene::add_sprite(Sprite *sprite)
{
    if (sprite == nullptr)
    {
        throw std::runtime_error("Sprite cannot be null");
    }

    sprites.push_back(sprite);
}

bool Scene::remove_sprite(Sprite *sprite)
{
    if (sprite == nullptr)
    {
        throw std::invalid_argument("Sprite cannot be null");
    }

    // Find the sprite starting from the END of the vector
    auto rit = std::find(sprites.rbegin(), sprites.rend(), sprite);

    if (rit == sprites.rend())
    {
        return false; // Sprite not found
    }

    // Convert reverse iterator to regular iterator for erasure
    // Note: base() points to the element AFTER the one the reverse iterator points to
    auto it = rit.base() - 1;
    
    // Remove the found element
    sprites.erase(it);
    return true; // Sprite removed successfully
}

void Scene::draw()
{
    if (!is_visible_flag)
    {
        return; // Do not draw if not visible
    }

    for (auto sprite : sprites)
    {
        /* Since sprites including children are
        in the childrenSprites, we don't have to call draw_all */
        sprite->draw_self(); // Draw all sprites in the scene
    }
}

void Scene::resize(double width_multiplier, double height_multiplier)
{
    if (width_multiplier <= 0 || height_multiplier <= 0)
    {
        throw std::invalid_argument("Width and height multipliers must be positive");
    }

    rect.w = static_cast<int>(rect.w * width_multiplier);
    rect.h = static_cast<int>(rect.h * height_multiplier);

    // Resize all sprites in the scene
    for (auto sprite : sprites)
    {
        sprite->resize_self(static_cast<int>(sprite->rect.w * width_multiplier),
                       static_cast<int>(sprite->rect.h * height_multiplier));
    }
}

void Scene::resize(int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        throw std::invalid_argument("Width and height must be positive");
    }

    double width_multiplier = static_cast<double>(width) / rect.w;
    double height_multiplier = static_cast<double>(height) / rect.h;
   
    Scene::resize(width_multiplier, height_multiplier);
}