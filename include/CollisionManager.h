// ============================================================
// CollisionManager.h — All collision / hit detection
//
// Checks three things every frame:
//   1. Player projectiles vs enemies → damage enemies
//   2. Enemy projectiles vs player  → damage player
//   3. Enemies touching player      → melee damage
//
// Also handles Ali's shockwave AOE and Ahaan's reveal pulse.
// ============================================================
#pragma once

#include <vector>
#include <memory>
#include "Player.h"

// Forward declarations
class Enemy;
class Projectile;
class AliPlayer;
class AhaanPlayer;

class CollisionManager
{
public:
    CollisionManager() = default;

    // Call this once per frame from Game::update()
    // enemies and projectiles come from SceneManager
    void update(Player* player,
                std::vector<std::shared_ptr<Enemy>>& enemies,
                std::vector<Projectile>& sceneProjectiles);

    // Check Ali's shockwave against all enemies
    void checkShockwave(AliPlayer* ali,
                        std::vector<std::shared_ptr<Enemy>>& enemies);

    // Check Ahaan's clone — enemies should prefer clone as target
    // Returns the best target position for enemies
    sf::Vector2f getBestPlayerTarget(AhaanPlayer* ahaan,
                                      Player* realPlayer);

private:
    // Returns true if two AABB rectangles overlap
    bool overlaps(const sf::FloatRect& a, const sf::FloatRect& b) const
    {
        return a.intersects(b);
    }

    float distance(sf::Vector2f a, sf::Vector2f b) const;
};