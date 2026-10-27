// ============================================================
// AliPlayer.h — Ali Raza (Telekinetic Warrior)
//
// Protocol 2 & 3 character focus.
//
// Abilities:
//   Left-click  → Telekinetic Punch (melee range knockback)
//   Right-click → Shockwave (AOE burst, pushes all nearby enemies)
//   Q           → Levitate (hold one enemy in the air briefly)
// ============================================================
#pragma once

#include "Player.h"
#include "Projectile.h"
#include <vector>

class AliPlayer : public Player
{
public:
    explicit AliPlayer(sf::Vector2f startPos);

    void update(float dt)              override;
    void draw(sf::RenderWindow& window) override;

    // ── Combat overrides ──────────────────────────────────────
    void primaryAttack(sf::Vector2f targetPos)    override; // Telekinetic punch
    void specialAbility(sf::Vector2f targetPos)   override; // Shockwave
    void secondaryAbility(sf::Vector2f targetPos) override; // Levitate

    // Projectiles fired this frame (CollisionManager reads these)
    std::vector<Projectile>& getProjectiles() { return m_projectiles; }

    // Shockwave radius (CollisionManager needs this)
    float getShockwaveRadius()  const { return m_shockwaveRadius; }
    bool  isShockwaveActive()   const { return m_shockwaveTimer > 0.f; }
    sf::Vector2f getShockwaveOrigin() const { return m_shockwaveOrigin; }

private:
    std::vector<Projectile> m_projectiles;

    // ── Shockwave state ───────────────────────────────────────
    float        m_shockwaveTimer{0.f};  // > 0 means active
    float        m_shockwaveRadius{0.f};
    sf::Vector2f m_shockwaveOrigin;

    // ── Visual effects ────────────────────────────────────────
    sf::CircleShape m_shockwaveRing;  // expanding ring drawn on screen

    void updateShockwave(float dt);
    void drawShockwave(sf::RenderWindow& window);
};