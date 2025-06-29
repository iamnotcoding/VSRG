#include "prefab.hpp"
#include <stdexcept>

using namespace prefab;

PreFab::PreFab(int x, int y):
    sprite::Sprite(nullptr, x, y, 0, 0) 
{

}

void PreFab::draw_self()
{
    throw std::runtime_error("Cannot call draw_self on a prefab");
}
