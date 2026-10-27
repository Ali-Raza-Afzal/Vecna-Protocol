// ============================================================
// Enemy.cpp — Base enemy logic
// ============================================================
#include "Enemy.h"
#include <cmath>
#include <cstdlib>  // rand
#include <iostream>

constexpr float DETECT_RADIUS = 350.f;   // pixels — when enemy starts chasing
constexpr float ATTACK_COOLDOWN = 1.5f;  // seconds between attacks
constexpr float HEALTH_BAR_W = 40.f;

// ── Constructor ──────────────────────────────────────────────
Enemy::Enemy(sf::Vector2f pos, const std::string& texturePath,
             float health, float speed, float damage, float attackRange)
    : m_health(health), m_maxHealth(health), m_speed(speed),
      m_damage(damage), m_attackRange(attackRange)
{
    if (!m_texture.loadFromFile(texturePath))
    {
        std::cerr << "[Enemy] Texture not found: " << texturePath
                  << " — using placeholder\n";
        sf::Image img;
        img.create(28, 40, sf::Color(200, 30, 30));
        m_texture.loadFromImage(img);
    }

    m_sprite.setTexture(m_texture);
    m_sprite.setOrigin(m_texture.getSize().x / 2.f,
                       m_texture.getSize().y / 2.f);
    m_sprite.setPosition(pos);

    // Health bar setup
    m_healthBarBg.setSize({HEALTH_BAR_W, 4.f});
    m_healthBarBg.setFillColor(sf::Color(60, 0, 0));
    m_healthBarFill.setFillColor(sf::Color(200, 30, 30));

    // Random initial wander direction
    m_wanderDir = {(float)(rand() % 3 - 1), (float)(rand() % 3 - 1)};
}

// ── Update ───────────────────────────────────────────────────
void Enemy::update(float dt, sf::Vector2f playerPos)
{
    if (m_state == EnemyState::Dead) return;

    updateStatusTimers(dt);
    if (m_attackCooldown > 0.f) m_attackCooldown -= dt;

    // State machine
    switch (m_state)
    {
    case EnemyState::Idle:
        wanderRandomly(dt);
        if (distanceTo(playerPos) < DETECT_RADIUS)
            m_state = EnemyState::Chase;
        break;

    case EnemyState::Chase:
        // If confused, wander randomly
        if (m_confuseTimer > 0.f)
            wanderRandomly(dt);
        else
            moveToward(playerPos, dt);

        // Close enough to attack?
        if (distanceTo(playerPos) < m_attackRange)
            m_state = EnemyState::Attack;
        break;

    case EnemyState::Attack:
        // Deal damage on cooldown
        if (m_attackCooldown <= 0.f)
        {
            // Actual damage is applied by CollisionManager
            // (it detects overlap with player bounds)
            m_attackCooldown = ATTACK_COOLDOWN;
        }
        // Back to chase if player moved away
        if (distanceTo(playerPos) > m_attackRange * 1.5f)
            m_state = EnemyState::Chase;
        break;

    case EnemyState::Stunned:
        // Do nothing while stunned (timer handled in updateStatusTimers)
        break;

    case EnemyState::Levitated:
        // Float upward slightly to show levitation
        m_sprite.move(0.f, -20.f * dt);
        break;

    case EnemyState::Dead:
        break;
    }

    m_animation.update(dt);
}

// ── Draw ─────────────────────────────────────────────────────
void Enemy::draw(sf::RenderWindow& window)
{
    if (m_state == EnemyState::Dead) return;

    window.draw(m_sprite);

    // Visual cues for status effects
    if (m_state == EnemyState::Stunned)
        m_sprite.setColor(sf::Color(255, 255, 100, 200)); // yellow flash
    else if (m_state == EnemyState::Levitated)
        m_sprite.setColor(sf::Color(150, 100, 255, 200)); // purple
    else
        m_sprite.setColor(sf::Color::White);

    drawHealthBar(window);
}

// ── Take damage ───────────────────────────────────────────────
void Enemy::takeDamage(float amount)
{
    m_health -= amount;
    if (m_health <= 0.f)
    {
        m_health = 0.f;
        m_state  = EnemyState::Dead;
    }
}

// ── Movement helpers ─────────────────────────────────────────
void Enemy::moveToward(sf::Vector2f target, float dt)
{
    sf::Vector2f dir = target - m_sprite.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0.f) return;
    dir /= len;
    m_sprite.move(dir * m_speed * dt);
}

void Enemy::wanderRandomly(float dt)
{
    m_wanderTimer -= dt;
    if (m_wanderTimer <= 0.f)
    {
        // Pick a new random direction every 2–4 seconds
        m_wanderTimer = 2.f + (rand() % 200) / 100.f;
        m_wanderDir = {
            (float)(rand() % 200 - 100) / 100.f,
            (float)(rand() % 200 - 100) / 100.f
        };
    }
    m_sprite.move(m_wanderDir * m_speed * 0.4f * dt);
}

// ── Status timer management ───────────────────────────────────
void Enemy::updateStatusTimers(float dt)
{
    if (m_stunTimer > 0.f)
    {
        m_stunTimer -= dt;
        if (m_stunTimer <= 0.f)
            m_state = EnemyState::Chase;
    }
    if (m_levTimer > 0.f)
    {
        m_levTimer -= dt;
        if (m_levTimer <= 0.f)
            m_state = EnemyState::Chase;
    }
    if (m_confuseTimer > 0.f)
        m_confuseTimer -= dt;
}

// ── Health bar ────────────────────────────────────────────────
void Enemy::drawHealthBar(sf::RenderWindow& window)
{
    sf::Vector2f pos = m_sprite.getPosition();
    float barX = pos.x - HEALTH_BAR_W / 2.f;
    float barY = pos.y - m_sprite.getGlobalBounds().height / 2.f - 8.f;

    m_healthBarBg.setPosition(barX, barY);
    window.draw(m_healthBarBg);

    float pct = m_health / m_maxHealth;
    m_healthBarFill.setSize({HEALTH_BAR_W * pct, 4.f});
    m_healthBarFill.setPosition(barX, barY);
    window.draw(m_healthBarFill);
}

// ── Distance helper ───────────────────────────────────────────
float Enemy::distanceTo(sf::Vector2f target) const
{
    sf::Vector2f diff = target - m_sprite.getPosition();
    return std::sqrt(diff.x * diff.x + diff.y * diff.y);
}