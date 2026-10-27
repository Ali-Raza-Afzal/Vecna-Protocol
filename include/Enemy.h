// ============================================================
// Enemy.h — Base enemy class
//
// Simple 3-state AI:
//   Idle    → wander slowly
//   Chase   → move toward player
//   Attack  → play attack animation, deal damage
//
// ShadowCreature, HiveMonster, and VecnaBoss inherit from this.
// ============================================================
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "Animation.h"

// Enemy states
enum class EnemyState { Idle, Chase, Attack, Stunned, Levitated, Dead };

// ============================================================
class Enemy
{
public:
    Enemy(sf::Vector2f pos, const std::string& texturePath,
          float health, float speed, float damage, float attackRange);
    virtual ~Enemy() = default;

    virtual void update(float dt, sf::Vector2f playerPos);
    virtual void draw(sf::RenderWindow& window);

    // Damage / death
    void  takeDamage(float amount);
    bool  isDead() const { return m_state == EnemyState::Dead; }

    // Stun effects (set by CollisionManager)
    void applyStun(float duration)    { m_stunTimer = duration; m_state = EnemyState::Stunned; }
    void applyLevitate(float duration){ m_levTimer  = duration; m_state = EnemyState::Levitated; }
    void applyConfuse(float duration) { m_confuseTimer = duration; }

    // Accessors
    sf::FloatRect getBounds() const { return m_sprite.getGlobalBounds(); }
    sf::Vector2f  getPosition() const { return m_sprite.getPosition(); }
    float getHealth()    const { return m_health; }
    float getMaxHealth() const { return m_maxHealth; }
    float getDamage()    const { return m_damage; }

protected:
    sf::Texture   m_texture;
    sf::Sprite    m_sprite;
    Animation     m_animation;

    float m_health;
    float m_maxHealth;
    float m_speed;
    float m_damage;
    float m_attackRange;   // distance to trigger Attack state

    EnemyState m_state{EnemyState::Idle};

    // Timers
    float m_attackCooldown{0.f};
    float m_stunTimer{0.f};
    float m_levTimer{0.f};
    float m_confuseTimer{0.f};
    float m_wanderTimer{0.f};
    sf::Vector2f m_wanderDir;

    // Health bar
    sf::RectangleShape m_healthBarBg;
    sf::RectangleShape m_healthBarFill;

    // Helpers
    void moveToward(sf::Vector2f target, float dt);
    void wanderRandomly(float dt);
    void updateStatusTimers(float dt);
    void drawHealthBar(sf::RenderWindow& window);
    float distanceTo(sf::Vector2f target) const;
};