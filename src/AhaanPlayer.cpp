// ============================================================
// AhaanPlayer.cpp  Ahaan Shuja's illusion abilities
// ============================================================

#include "AhaanPlayer.h"
#include <cmath>
#include <iostream>
#include <algorithm>
constexpr float PRIMARY_CD    = 0.4f;
constexpr float SPECIAL_CD    = 4.0f;
constexpr float SECONDARY_CD  = 8.0f;

constexpr float PRIMARY_COST   = 8.f;
constexpr float SPECIAL_COST   = 25.f;
constexpr float SECONDARY_COST = 35.f;

constexpr float REVEAL_DURATION = 3.0f;   // seconds the reveal lasts
constexpr float CLONE_LIFETIME  = 6.0f;   // seconds clone stays alive

AhaanPlayer::AhaanPlayer(sf::Vector2f startPos)
    : Player(startPos, "assets/textures/ahaan.png")
{
    m_name = "Ahaan Shuja";

    // Reveal ring visual ” purple pulse
    m_revealRing.setFillColor(sf::Color::Transparent);
    m_revealRing.setOutlineColor(sf::Color(180, 80, 255, 180));
    m_revealRing.setOutlineThickness(4.f);

    // Clone sprite reuses same texture
    m_cloneSprite.setTexture(m_texture);
    m_cloneSprite.setOrigin(
        m_texture.getSize().x / 2.f,
        m_texture.getSize().y / 2.f);
    // Tint clone to look ghostly
    m_cloneSprite.setColor(sf::Color(180, 120, 255, 160));
}

void AhaanPlayer::update(float dt)
{
    Player::update(dt);
    updateReveal(dt);
    updateClone(dt);

    // Clean & update projectiles
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const Projectile& p) { return p.isDead(); }),
        m_projectiles.end());
    for (auto& p : m_projectiles)
        p.update(dt);
}

void AhaanPlayer::draw(sf::RenderWindow& window)
{
    drawClone(window);          // draw clone behind real player
    Player::draw(window);
    drawReveal(window);
    for (auto& p : m_projectiles)
        p.draw(window);
}

// A fast purple bolt that deals moderate damage + briefly
// confuses the enemy (causes it to wander randomly for 1.5s).
void AhaanPlayer::primaryAttack(sf::Vector2f targetPos)
{
    if (m_primaryCooldown > 0.f) return;
    if (!spendEnergy(PRIMARY_COST)) return;

    m_primaryCooldown = PRIMARY_CD;

    sf::Vector2f origin = m_sprite.getPosition();
    sf::Vector2f dir    = targetPos - origin;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0.f) return;
    dir /= len;

    Projectile bolt;
    bolt.init(origin, dir * 550.f,
              sf::Color(200, 80, 255),
              10.f,
              12.f,      // damage
              1.5f,
              true);
    bolt.setIsIllusion(true);   // CollisionManager applies confuse effect
    m_projectiles.push_back(bolt);

    faceToward(targetPos);
}

// Emits a wide purple pulse that lasts REVEAL_DURATION seconds.
// While active, SceneManager reveals hidden paths / fake walls.
// Also deals small damage to all illusion-type enemies.
void AhaanPlayer::specialAbility(sf::Vector2f /*targetPos*/)
{
    if (m_specialCooldown > 0.f) return;
    if (!spendEnergy(SPECIAL_COST)) return;

    m_specialCooldown = SPECIAL_CD;
    m_revealTimer     = REVEAL_DURATION;

    std::cout << "[Ahaan] TRUTH REVEAL activated!\n";
}

// Places a ghost copy of Ahaan at a chosen position.
// Enemies will target the clone instead of the real player.
// Clone disappears after CLONE_LIFETIME seconds.
void AhaanPlayer::secondaryAbility(sf::Vector2f targetPos)
{
    if (m_secondaryCooldown > 0.f) return;
    if (!spendEnergy(SECONDARY_COST)) return;

    m_secondaryCooldown = SECONDARY_CD;
    m_cloneAlive        = true;
    m_cloneLifetime     = CLONE_LIFETIME;
    m_cloneSprite.setPosition(targetPos);
    m_cloneFlicker      = 0.f;

    std::cout << "[Ahaan] Clone placed at "
              << targetPos.x << ", " << targetPos.y << "\n";
}

void AhaanPlayer::updateReveal(float dt)
{
    if (m_revealTimer <= 0.f) return;
    m_revealTimer -= dt;

    // Expand the ring visual
    float progress = 1.f - (m_revealTimer / REVEAL_DURATION);
    float radius   = progress * 400.f;   // expands to 400px
    sf::Uint8 alpha = static_cast<sf::Uint8>(180.f * (1.f - progress));

    m_revealRing.setRadius(radius);
    m_revealRing.setOrigin(radius, radius);
    m_revealRing.setPosition(m_sprite.getPosition());
    m_revealRing.setOutlineColor(sf::Color(180, 80, 255, alpha));
}

void AhaanPlayer::updateClone(float dt)
{
    if (!m_cloneAlive) return;

    m_cloneLifetime -= dt;
    m_cloneFlicker  += dt * 8.f;   // flicker speed

    // Flicker alpha to signal it's fading
    float flickerAlpha = 160.f + 80.f * std::sin(m_cloneFlicker);
    // Near end of life, fade out
    if (m_cloneLifetime < 1.5f)
        flickerAlpha *= (m_cloneLifetime / 1.5f);

    m_cloneSprite.setColor(sf::Color(
        180, 120, 255,
        static_cast<sf::Uint8>(flickerAlpha)));

    if (m_cloneLifetime <= 0.f)
        m_cloneAlive = false;
}

void AhaanPlayer::drawReveal(sf::RenderWindow& window)
{
    if (m_revealTimer > 0.f)
        window.draw(m_revealRing);
}


void AhaanPlayer::drawClone(sf::RenderWindow& window)
{
    if (m_cloneAlive)
        window.draw(m_cloneSprite);
}
