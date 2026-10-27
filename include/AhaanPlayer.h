// ============================================================
// AhaanPlayer.h — Ahaan Shuja (Illusion Master)
//
// Protocol 1 & 3 character focus.
//
// Abilities:
//   Left-click  → Illusion Blast (confuses / damages enemies)
//   Right-click → Truth Reveal (unmasks fake walls in Protocol 1)
//   Q           → Clone (spawns a short-lived decoy that draws fire)
// ============================================================
#pragma once

#include "Player.h"
#include "Projectile.h"
#include <vector>
#include <memory>

class AhaanPlayer : public Player
{
public:
    explicit AhaanPlayer(sf::Vector2f startPos);

    void update(float dt)               override;
    void draw(sf::RenderWindow& window) override;

    void primaryAttack(sf::Vector2f targetPos)    override; // Illusion blast
    void specialAbility(sf::Vector2f targetPos)   override; // Truth Reveal (AOE)
    void secondaryAbility(sf::Vector2f targetPos) override; // Clone

    std::vector<Projectile>& getProjectiles() { return m_projectiles; }

    // Is the Truth Reveal pulse active? (SceneManager checks to unhide fake walls)
    bool isRevealActive() const { return m_revealTimer > 0.f; }

    // Is the clone alive?
    bool isCloneAlive()   const { return m_cloneAlive; }
    sf::Vector2f getClonePosition() const { return m_cloneSprite.getPosition(); }

private:
    std::vector<Projectile> m_projectiles;

    // ── Truth Reveal ──────────────────────────────────────────
    float         m_revealTimer{0.f};    // > 0 = revealing
    sf::CircleShape m_revealRing;        // visual pulse

    // ── Illusion Clone ────────────────────────────────────────
    bool          m_cloneAlive{false};
    float         m_cloneLifetime{0.f};
    sf::Sprite    m_cloneSprite;         // reuses Ahaan's texture
    float         m_cloneFlicker{0.f};   // flicker alpha timer

    void updateReveal(float dt);
    void updateClone(float dt);
    void drawReveal(sf::RenderWindow& window);
    void drawClone(sf::RenderWindow& window);
};