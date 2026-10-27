// ============================================================
// Projectile.cpp
// ============================================================
#include "Projectile.h"

void Projectile::init(sf::Vector2f pos, sf::Vector2f velocity,
                      sf::Color color, float radius, float damage,
                      float lifetime, bool isPlayerOwned)
{
    m_shape.setRadius(radius);
    m_shape.setOrigin(radius, radius);
    m_shape.setPosition(pos);
    m_shape.setFillColor(color);

    // Add a subtle glow ring
    m_shape.setOutlineColor(sf::Color(
        static_cast<sf::Uint8>(color.r * 0.6f),
        static_cast<sf::Uint8>(color.g * 0.6f),
        static_cast<sf::Uint8>(color.b * 0.6f), 120));
    m_shape.setOutlineThickness(2.f);

    m_velocity    = velocity;
    m_damage      = damage;
    m_lifetime    = lifetime;
    m_playerOwned = isPlayerOwned;
    m_dead        = false;
    m_age         = 0.f;
}

void Projectile::update(float dt)
{
    if (m_dead) return;

    m_age += dt;
    if (m_age >= m_lifetime)
    {
        m_dead = true;
        return;
    }

    m_shape.move(m_velocity * dt);

    // Kill if it leaves the screen
    sf::Vector2f pos = m_shape.getPosition();
    if (pos.x < -50.f || pos.x > 1330.f ||
        pos.y < -50.f || pos.y > 770.f)
    {
        m_dead = true;
    }
}

void Projectile::draw(sf::RenderWindow& window) const
{
    if (!m_dead)
        window.draw(m_shape);
}