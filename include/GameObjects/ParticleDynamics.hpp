

#include <allegro5/color.h>
#include "GameObjects/GameObject.hpp"

namespace mg8
{

  class GravityWell : public virtual GameObject, public circle
  {

  private:
    vec2f velocity_to_well(const GameObject &body, float dt) const;
    vec2f euler(const GameObject &obj, float dt) const;
    vec2f kutta(const GameObject &obj, float dt) const;

  public:
    ALLEGRO_COLOR m_color = {0, 0, 0, 255};

    GravityWell(
        MG8_OBJECT_TYPES type = MG8_OBJECT_TYPES::TYPE_GRAVITY_WELL,
        vec2f position = {0, 0},
        vec2f velocity = {0, 0},
        float radius = hole_radius,
        ALLEGRO_COLOR color = {0, 255 / 2, 0, 255},
        uint32_t collision = 0);

    ~GravityWell();

    float m_mass = 6.0e+18;

    virtual void draw() const;
    virtual void move(vec2f delta_move);

    void apply(std::vector<GameObject *> &objs, float dt) const;
  };
}