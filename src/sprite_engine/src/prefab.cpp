#include "prefab.hpp"
#include <stdexcept>

using namespace prefab;

PreFab::PreFab(int x, int y):
    sprite::Sprite(nullptr, x, y, 0, 0) 
{

}

// nop since draw_all will call this
void PreFab::draw_self()
{
}

// nop since resize_all will call this
void PreFab::resize_self([[maybe_unused]]float width, [[maybe_unused]]float height)
{
}