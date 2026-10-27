// ============================================================
// AliPlayer.cpp — Ali Raza's abilities
// ============================================================

#include "AliPlayer.h"
#include <cmath>
#include <iostream>

// ── Cooldown constants (seconds) ─────────────────────────────
constexpr float PRIMARY_CD   = 0.3f;   // Telekinetic punch
constexpr float SPECIAL_CD   = 3.0f;   // Shockwave
constexpr float SECONDARY_CD = 5.0f;   // Levitate

// ── Energy costs ─────────────────────────────────────────────
constexpr float PRIMARY_COST   = 5.f;
constexpr float SPECIAL_COST   = 30.f;
constexpr float SECONDARY_COST = 20.f;

// ── Shockwave parameters ──────────────────────────────────────
constexpr float SHOCKWAVE_DURATION    = 0.4f;   // seconds the ring expands
constexpr float SHOCKWAVE_MAX_RADIUS  = 250.f;  // pixels

// ── Constructor ──────────────────────────────────────────────
AliPlayer::AliPlayer(sf::Vector2f startPos)
    : Player(startPos, "assets/textures/ali.png")
{
    m_name = "Ali Raza";

    // Shockwave ring visual setup
    m_shockwaveRing.setFillColor(sf::Color::Transparent);
    m_shockwaveRing.setOutlineColor(sf::Color(100, 180, 255, 200));
    m_shockwaveRing.setOutlineThickness(3.f);
}

// ── Update ───────────────────────────────────────────────────
void AliPlayer::update(float dt)
{
    Player::update(dt);            // base: movement, regen, cooldowns
    updateShockwave(dt);

    // Clean up expired projectiles
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const Projectile& p) { return p.isDead(); }),
        m_projectiles.end());

    // Update surviving projectiles
    for (auto& p : m_projectiles)
        p.update(dt);
}

// ── Draw ─────────────────────────────────────────────────────
void AliPlayer::draw(sf::RenderWindow& window)
{
    Player::draw(window);
    drawShockwave(window);

    for (auto& p : m_projectiles)
        p.draw(window);
}

// ── Primary Attack: Telekinetic Punch ────────────────────────
// Fires a fast blue projectile toward the mouse cursor.
// Low cooldown, low cost. Knocks enemies back slightly.
void AliPlayer::primaryAttack(sf::Vector2f targetPos)
{
    if (m_primaryCooldown > 0.f) return;
    if (!spendEnergy(PRIMARY_COST)) return;

    m_primaryCooldown = PRIMARY_CD;

    sf::Vector2f origin = m_sprite.getPosition();
    sf::Vector2f dir = targetPos - origin;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0.f) return;
    dir /= len;

    // Spawn a telekinetic bolt
    Projectile bolt;
    bolt.init(origin, dir * 600.f,   // speed = 600 px/s
              sf::Color(80, 160, 255),
              8.f,                    // size
              10.f,                   // damage
              1.5f,                   // lifetime seconds
              true);                  // isPlayerProjectile
    m_projectiles.push_back(bolt);

    faceToward(targetPos);
}

// ── Special: Shockwave ────────────────────────────────────────
// Emits a ring from Ali's position that damages + knocks back all
// nearby enemies. CollisionManager reads isShockwaveActive().
void AliPlayer::specialAbility(sf::Vector2f /*targetPos*/)
{
    if (m_specialCooldown > 0.f) return;
    if (!spendEnergy(SPECIAL_COST)) return;

    m_specialCooldown = SPECIAL_CD;
    m_shockwaveTimer  = SHOCKWAVE_DURATION;
    m_shockwaveRadius = 0.f;
    m_shockwaveOrigin = m_sprite.getPosition();

    std::cout << "[Ali] SHOCKWAVE!\n";
}

// ── Secondary: Levitate ───────────────────────────────────────
// Fires a slower "levitate beam" — enemies it hits are stunned/
// lifted. Simple implementation: just a slow projectile that
// sets a "levitated" flag on the first enemy it hits.
void AliPlayer::secondaryAbility(sf::Vector2f targetPos)
{
    if (m_secondaryCooldown > 0.f) return;
    if (!spendEnergy(SECONDARY_COST)) return;

    m_secondaryCooldown = SECONDARY_CD;

    sf::Vector2f origin = m_sprite.getPosition();
    sf::Vector2f dir    = targetPos - origin;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0.f) return;
    dir /= len;

    Projectile beam;
    beam.init(origin, dir * 350.f,
              sf::Color(200, 100, 255),
              14.f,            // larger visual
              5.f,             // less damage — the stun is the point
              2.0f,
              true);
    beam.setIsLevitate(true);  // CollisionManager will apply stun
    m_projectiles.push_back(beam);

    std::cout << "[Ali] Levitate beam fired\n";
}

// ── Shockwave ring update ────────────────────────────────────
void AliPlayer::updateShockwave(float dt)
{
    if (m_shockwaveTimer <= 0.f) return;

    m_shockwaveTimer -= dt;
    float progress = 1.f - (m_shockwaveTimer / SHOCKWAVE_DURATION);
    m_shockwaveRadius = progress * SHOCKWAVE_MAX_RADIUS;

    // The ring fades as it expands
    sf::Uint8 alpha = static_cast<sf::Uint8>(200.f * (1.f - progress));
    m_shockwaveRing.setOutlineColor(sf::Color(100, 180, 255, alpha));
    m_shockwaveRing.setRadius(m_shockwaveRadius);
    m_shockwaveRing.setOrigin(m_shockwaveRadius, m_shockwaveRadius);
    m_shockwaveRing.setPosition(m_shockwaveOrigin);
}

// ── Shockwave ring draw ───────────────────────────────────────
void AliPlayer::drawShockwave(sf::RenderWindow& window)
{
    if (m_shockwaveTimer > 0.f)
        window.draw(m_shockwaveRing);
}