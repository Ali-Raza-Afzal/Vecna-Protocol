// ============================================================
// Player.h — Base player class
//
// AliPlayer and AhaanPlayer both inherit from this.
// Contains everything both characters share:
//   - movement (WASD)
//   - health / energy bars
//   - animation system reference
//   - abstract attack methods (overridden by children)
// ============================================================
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "Animation.h"

// ── Constants you can tweak easily ───────────────────────────
constexpr float PLAYER_MAX_HEALTH = 100.f;
constexpr float PLAYER_MAX_ENERGY = 100.f;
constexpr float PLAYER_SPEED      = 200.f;  // pixels per second

// ============================================================
class Player
{
public:
    Player(sf::Vector2f startPos, const std::string& texturePath);
    virtual ~Player() = default;

    // ── Called every frame ────────────────────────────────────
    virtual void update(float dt);
    virtual void draw(sf::RenderWindow& window);

    // ── Combat — pure virtual (must be overridden) ────────────
    virtual void primaryAttack(sf::Vector2f targetPos)   = 0;
    virtual void specialAbility(sf::Vector2f targetPos)  = 0;
    virtual void secondaryAbility(sf::Vector2f targetPos)= 0;

    // ── Health / Energy accessors ─────────────────────────────
    void  takeDamage(float amount);
    bool  isAlive()     const { return m_health > 0.f; }
    float getHealth()   const { return m_health; }
    float getMaxHealth()const { return PLAYER_MAX_HEALTH; }
    float getEnergy()   const { return m_energy; }
    float getMaxEnergy()const { return PLAYER_MAX_ENERGY; }

    // ── Position ──────────────────────────────────────────────
    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }
    sf::FloatRect getBounds()  const { return m_sprite.getGlobalBounds(); }

    // ── Identity (shown in HUD) ───────────────────────────────
    const std::string& getName() const { return m_name; }

protected:
    // ── SFML visuals ──────────────────────────────────────────
    sf::Texture      m_texture;
    sf::Sprite       m_sprite;

    // ── Stats ─────────────────────────────────────────────────
    float m_health{PLAYER_MAX_HEALTH};
    float m_energy{PLAYER_MAX_ENERGY};
    float m_speed{PLAYER_SPEED};

    // ── Ability cooldowns (seconds remaining) ─────────────────
    float m_primaryCooldown{0.f};
    float m_specialCooldown{0.f};
    float m_secondaryCooldown{0.f};

    // ── Animation ─────────────────────────────────────────────
    Animation m_animation;

    // ── Identity ──────────────────────────────────────────────
    std::string m_name;

    // ── Protected helpers ─────────────────────────────────────
    void handleMovement(float dt);
    void regenEnergy(float dt);      // slowly refills energy over time
    void tickCooldowns(float dt);    // counts down all cooldowns
    bool spendEnergy(float amount);  // returns false if not enough energy
    void faceToward(sf::Vector2f target); // flip sprite to face mouse
};