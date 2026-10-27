// ============================================================
// CollisionManager.cpp
// ============================================================

#include "CollisionManager.h"
#include "Enemy.h"
#include "Projectile.h"
#include "AliPlayer.h"
#include "AhaanPlayer.h"
#include <cmath>

void CollisionManager::update(Player* player,
                               std::vector<std::shared_ptr<Enemy>>& enemies,
                               std::vector<Projectile>& sceneProjectiles)
{
    if (!player) return;
    sf::FloatRect playerBounds = player->getBounds();

    for (auto& enemy : enemies)
    {
        if (enemy->isDead()) continue;

        sf::FloatRect enemyBounds = enemy->getBounds();

        // ── Enemy melee vs player ─────────────────────────────
        if (overlaps(playerBounds, enemyBounds))
        {
            // Enemy hits player once per attack cycle
            // (Enemy tracks its own attack cooldown)
            player->takeDamage(enemy->getDamage() * 0.016f); // damage per frame ~= 1s / 60fps
        }
    }

    // ── Player projectiles vs enemies ─────────────────────────
    // We need to know who the player is to get their projectiles.
    // Downcast safely to check for AliPlayer or AhaanPlayer.

    auto* ali   = dynamic_cast<AliPlayer*>(player);
    auto* ahaan = dynamic_cast<AhaanPlayer*>(player);

    auto checkProjectilesVsEnemies = [&](std::vector<Projectile>& projs)
    {
        for (auto& proj : projs)
        {
            if (proj.isDead() || !proj.isPlayerOwned()) continue;

            for (auto& enemy : enemies)
            {
                if (enemy->isDead()) continue;

                if (overlaps(proj.getBounds(), enemy->getBounds()))
                {
                    enemy->takeDamage(proj.getDamage());

                    // Apply special effects
                    if (proj.isIllusionType())
                        enemy->applyConfuse(1.5f);   // 1.5s confusion
                    if (proj.isLevitateType())
                        enemy->applyLevitate(2.0f);  // 2s levitation

                    proj.kill();
                    break;  // one hit per projectile
                }
            }
        }
    };

    if (ali)   checkProjectilesVsEnemies(ali->getProjectiles());
    if (ahaan) checkProjectilesVsEnemies(ahaan->getProjectiles());

    // ── Enemy projectiles vs player ───────────────────────────
    for (auto& proj : sceneProjectiles)
    {
        if (proj.isDead() || proj.isPlayerOwned()) continue;

        if (overlaps(proj.getBounds(), playerBounds))
        {
            player->takeDamage(proj.getDamage());
            proj.kill();
        }
    }

    // ── Ali shockwave ─────────────────────────────────────────
    if (ali && ali->isShockwaveActive())
        checkShockwave(ali, enemies);
}

void CollisionManager::checkShockwave(AliPlayer* ali,
    std::vector<std::shared_ptr<Enemy>>& enemies)
{
    sf::Vector2f origin = ali->getShockwaveOrigin();
    float        radius = ali->getShockwaveRadius();

    for (auto& enemy : enemies)
    {
        if (enemy->isDead()) continue;

        float dist = distance(origin, enemy->getPosition());
        if (dist < radius)
        {
            // Damage falls off with distance
            float falloff = 1.f - (dist / radius);
            enemy->takeDamage(30.f * falloff);
            enemy->applyStun(0.5f);   // brief stun from the blast
        }
    }
}

sf::Vector2f CollisionManager::getBestPlayerTarget(
    AhaanPlayer* ahaan, Player* realPlayer)
{
    // If clone is alive, enemies target it instead
    if (ahaan && ahaan->isCloneAlive())
        return ahaan->getClonePosition();

    return realPlayer->getPosition();
}

float CollisionManager::distance(sf::Vector2f a, sf::Vector2f b) const
{
    sf::Vector2f d = b - a;
    return std::sqrt(d.x * d.x + d.y * d.y);
}