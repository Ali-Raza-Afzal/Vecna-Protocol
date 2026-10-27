// ============================================================
// VecnaBoss.cpp — Final boss implementation
// ============================================================

#include "VecnaBoss.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

// ── Constructor ──────────────────────────────────────────────
VecnaBoss::VecnaBoss(sf::Vector2f pos)
    : Enemy(pos, "assets/textures/vecna.png",
            500.f,   // health
            80.f,    // speed (slower but hits hard)
            25.f,    // melee damage
            80.f)    // attack range
{
    // Make Vecna larger and more ominous
    m_sprite.setScale(2.f, 2.f);
    m_sprite.setColor(sf::Color(160, 30, 200));  // purple tint

    // Darkness overlay for phase 3
    m_darknessOverlay.setSize({1280.f, 720.f});
    m_darknessOverlay.setFillColor(sf::Color(0, 0, 0, 0));

    // Tentacle shape
    m_tentacle.setSize({200.f, 12.f});
    m_tentacle.setFillColor(sf::Color(100, 0, 160, 200));
    m_tentacle.setOrigin(0.f, 6.f);
}

// ── Update ───────────────────────────────────────────────────
void VecnaBoss::update(float dt, sf::Vector2f playerPos)
{
    if (isDefeated()) return;

    // Tick inherited status timers
    updateStatusTimers(dt);
    if (m_attackCooldown > 0.f) m_attackCooldown -= dt;
    if (m_teleportCooldown > 0.f) m_teleportCooldown -= dt;
    if (m_rangedCooldown   > 0.f) m_rangedCooldown   -= dt;
    if (m_tentacleCooldown > 0.f) m_tentacleCooldown -= dt;
    if (m_tentacleTimer    > 0.f)
    {
        m_tentacleTimer -= dt;
        if (m_tentacleTimer <= 0.f) m_tentacleActive = false;
    }

    checkPhaseTransition();

    // Update boss projectiles
    m_bossProjectiles.erase(
        std::remove_if(m_bossProjectiles.begin(), m_bossProjectiles.end(),
            [](const Projectile& p){ return p.isDead(); }),
        m_bossProjectiles.end());
    for (auto& p : m_bossProjectiles)
        p.update(dt);

    // Phase-specific behaviour
    switch (m_phase)
    {
    case BossPhase::Phase1: phase1Behaviour(dt, playerPos); break;
    case BossPhase::Phase2: phase2Behaviour(dt, playerPos); break;
    case BossPhase::Phase3: phase3Behaviour(dt, playerPos); break;
    default: break;
    }
}

// ── Draw ─────────────────────────────────────────────────────
void VecnaBoss::draw(sf::RenderWindow& window)
{
    if (isDefeated()) return;

    // Phase 3 darkness
    if (m_phase == BossPhase::Phase3)
    {
        m_darkAlpha = std::min(m_darkAlpha + 0.5f, 140.f);
        m_darknessOverlay.setFillColor(
            sf::Color(0, 0, 0, static_cast<sf::Uint8>(m_darkAlpha)));
        window.draw(m_darknessOverlay);
    }

    // Tentacle
    if (m_tentacleActive)
        window.draw(m_tentacle);

    Enemy::draw(window);  // sprite + health bar

    for (auto& p : m_bossProjectiles)
        p.draw(window);
}

// ── Phase checks ─────────────────────────────────────────────
void VecnaBoss::checkPhaseTransition()
{
    float pct = m_health / m_maxHealth;

    if (m_phase == BossPhase::Phase1 && pct <= 0.66f)
    {
        m_phase = BossPhase::Phase2;
        m_speed *= 1.2f;     // gets faster
        std::cout << "[Vecna] PHASE 2 — teleport begins!\n";
    }
    else if (m_phase == BossPhase::Phase2 && pct <= 0.33f)
    {
        m_phase = BossPhase::Phase3;
        m_speed *= 1.3f;
        std::cout << "[Vecna] PHASE 3 — FINAL FORM!\n";
    }
}

// ── Phase 1: Chase + ranged attacks ──────────────────────────
void VecnaBoss::phase1Behaviour(float dt, sf::Vector2f playerPos)
{
    moveToward(playerPos, dt);
    fireRangedAttack(playerPos);
}

// ── Phase 2: Adds teleport + occasional summon ────────────────
void VecnaBoss::phase2Behaviour(float dt, sf::Vector2f playerPos)
{
    moveToward(playerPos, dt);
    fireRangedAttack(playerPos);

    // Teleport occasionally
    if (m_teleportCooldown <= 0.f)
    {
        doTeleport(playerPos);
        m_teleportCooldown = 5.f;
    }
}

// ── Phase 3: Rage mode — darkness + tentacles ────────────────
void VecnaBoss::phase3Behaviour(float dt, sf::Vector2f playerPos)
{
    moveToward(playerPos, dt);
    fireRangedAttack(playerPos);

    if (m_teleportCooldown <= 0.f)
    {
        doTeleport(playerPos);
        m_teleportCooldown = 3.f;  // shorter in phase 3
    }

    if (m_tentacleCooldown <= 0.f)
    {
        doTentacleStrike(playerPos);
        m_tentacleCooldown = 4.f;
    }
}

// ── Teleport ─────────────────────────────────────────────────
void VecnaBoss::doTeleport(sf::Vector2f nearPlayer)
{
    // Jump to a random position within 300px of the player
    float angle = (rand() % 360) * 3.14159f / 180.f;
    float dist  = 150.f + rand() % 150;
    sf::Vector2f newPos = nearPlayer + sf::Vector2f(
        std::cos(angle) * dist, std::sin(angle) * dist);

    // Clamp to screen
    newPos.x = std::max(100.f, std::min(newPos.x, 1180.f));
    newPos.y = std::max(100.f, std::min(newPos.y, 620.f));

    m_sprite.setPosition(newPos);
    std::cout << "[Vecna] Teleported!\n";
}

// ── Ranged attack ─────────────────────────────────────────────
void VecnaBoss::fireRangedAttack(sf::Vector2f playerPos)
{
    if (m_rangedCooldown > 0.f) return;
    m_rangedCooldown = (m_phase == BossPhase::Phase3) ? 1.5f : 2.5f;

    sf::Vector2f origin = m_sprite.getPosition();
    sf::Vector2f dir    = playerPos - origin;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0.f) return;
    dir /= len;

    Projectile bolt;
    bolt.init(origin, dir * 300.f,
              sf::Color(120, 0, 180),
              12.f,
              15.f,     // damage
              3.f,
              false);   // isPlayerOwned = false → hits player
    m_bossProjectiles.push_back(bolt);
}

// ── Tentacle strike ──────────────────────────────────────────
void VecnaBoss::doTentacleStrike(sf::Vector2f playerPos)
{
    sf::Vector2f origin = m_sprite.getPosition();
    sf::Vector2f dir    = playerPos - origin;
    float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;

    m_tentacle.setPosition(origin);
    m_tentacle.setRotation(angle);
    m_tentacleActive = true;
    m_tentacleTimer  = 0.4f;  // visible for 0.4 seconds

    std::cout << "[Vecna] TENTACLE STRIKE!\n";
}

// ── Final trap (Ali + Ahaan combined power) ───────────────────
// Returns true if the boss is defeated
bool VecnaBoss::applyFinalTrap()
{
    m_trapResistance -= 0.25f;
    std::cout << "[Vecna] Trap resistance: " << m_trapResistance << "\n";

    if (m_trapResistance <= 0.f)
    {
        m_health = 0.f;
        m_phase  = BossPhase::Defeated;
        std::cout << "[Vecna] DEFEATED!\n";
        return true;
    }
    return false;
}