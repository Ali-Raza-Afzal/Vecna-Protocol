// ============================================================
// VecnaBoss.h — Protocol 3 final boss
//
// Three phases:
//   Phase 1 (100-66% HP): Simple chase + ranged attacks
//   Phase 2 (66-33% HP): Adds teleport + summon minions
//   Phase 3 (<33% HP)  : Full darkness, screen effects,
//                         tentacle strikes, desperate attacks
//
// The boss becomes "trapped" when both Ali's shockwave AND
// Ahaan's reveal are active simultaneously — this is the
// win condition for Protocol 3.
// ============================================================
#pragma once

#include "Enemy.h"
#include "Projectile.h"
#include <vector>

enum class BossPhase { Phase1, Phase2, Phase3, Defeated };

// ============================================================
class VecnaBoss : public Enemy
{
public:
    explicit VecnaBoss(sf::Vector2f pos);

    void update(float dt, sf::Vector2f playerPos)  override;
    void draw(sf::RenderWindow& window)             override;

    // Boss-specific accessors
    BossPhase    getPhase()       const { return m_phase; }
    bool         isDefeated()     const { return m_phase == BossPhase::Defeated; }
    bool         isSummoning()    const { return m_summonTimer > 0.f; }
    std::vector<Projectile>& getBossProjectiles()  { return m_bossProjectiles; }

    // Call this when Ali + Ahaan use their powers simultaneously
    // Returns true if this kills Vecna
    bool applyFinalTrap();

private:
    BossPhase m_phase{BossPhase::Phase1};

    std::vector<Projectile> m_bossProjectiles;

    // Phase timers / cooldowns
    float m_teleportCooldown{0.f};
    float m_summonTimer{0.f};
    float m_tentacleCooldown{0.f};
    float m_rangedCooldown{0.f};
    float m_trapResistance{1.f};    // decreases as boss is weakened

    // Screen darkness effect (phase 3)
    sf::RectangleShape m_darknessOverlay;
    float m_darkAlpha{0.f};

    // Tentacle visual (simple rectangle sweeping from Vecna)
    sf::RectangleShape m_tentacle;
    bool  m_tentacleActive{false};
    float m_tentacleTimer{0.f};

    // ── Phase logic ───────────────────────────────────────────
    void checkPhaseTransition();
    void phase1Behaviour(float dt, sf::Vector2f playerPos);
    void phase2Behaviour(float dt, sf::Vector2f playerPos);
    void phase3Behaviour(float dt, sf::Vector2f playerPos);

    void doTeleport(sf::Vector2f nearPlayer);
    void fireRangedAttack(sf::Vector2f playerPos);
    void doTentacleStrike(sf::Vector2f playerPos);
};