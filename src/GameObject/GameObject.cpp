
#include "GameObject/GameObject.hpp"
#include "math/vector.hpp"

namespace mg8
{

  GameObject::GameObject(const vec2 coords)
  {
    _coords = coords;
  }
  GameObject::GameObject(const float x, const float y)
  {
    _coords.x = x;
    _coords.y = y;
  }
}