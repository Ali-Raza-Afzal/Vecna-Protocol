// ============================================================
// Projectile.h — A fired bolt / beam / blast
//
// Keeps track of position, velocity, damage, and lifetime.
// CollisionManager checks player projectiles against enemies
// and enemy projectiles against the player.
// ============================================================
#pragma once

#include <SFML/Graphics.hpp>

class Projectile
{
public:
    Projectile() = default;

    // Call this right after construction to fully configure the bolt
    void init(sf::Vector2f pos,
              sf::Vector2f velocity,
              sf::Color    color,
              float        radius,
              float        damage,
              float        lifetime,
              bool         isPlayerOwned);

    void update(float dt);
    void draw(sf::RenderWindow& window) const;

    // State checks
    bool isDead()            const { return m_dead; }
    bool isPlayerOwned()     const { return m_playerOwned; }
    bool isIllusionType()    const { return m_isIllusion; }
    bool isLevitateType()    const { return m_isLevitate; }

    float  getDamage()       const { return m_damage; }
    sf::FloatRect getBounds()const { return m_shape.getGlobalBounds(); }

    // Flags set by character classes
    void setIsIllusion(bool v) { m_isIllusion = v; }
    void setIsLevitate(bool v) { m_isLevitate = v; }

    // Called by CollisionManager when it hits something
    void kill() { m_dead = true; }

private:
    sf::CircleShape  m_shape;
    sf::Vector2f     m_velocity;
    float            m_damage{10.f};
    float            m_lifetime{2.f};   // seconds until auto-expire
    float            m_age{0.f};
    bool             m_dead{false};
    bool             m_playerOwned{true};
    bool             m_isIllusion{false};
    bool             m_isLevitate{false};
};