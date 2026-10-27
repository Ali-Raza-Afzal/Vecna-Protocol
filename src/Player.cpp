// ============================================================
// Player.cpp  Base player implementation
//
// Movement, energy regen, cooldown ticking.
// Attack logic lives in AliPlayer / AhaanPlayer.
// ============================================================

#include "Player.h"
#include <iostream>
#include <cmath>

Player::Player(sf::Vector2f startPos, const std::string& texturePath)
{
    
    if (!m_texture.loadFromFile(texturePath))
    {
        std::cerr << "[Player] Texture not found: " << texturePath
                  << " — using placeholder\n";
        // Create a 32×48 placeholder texture (magenta)
        sf::Image img;
        img.create(32, 48, sf::Color(200, 50, 200));
        m_texture.loadFromImage(img);
    }

    m_sprite.setTexture(m_texture);
    // Centre the origin so rotation and positioning feel natural
    m_sprite.setOrigin(
        m_texture.getSize().x / 2.f,
        m_texture.getSize().y / 2.f);
    m_sprite.setPosition(startPos);
}

void Player::update(float dt)
{
    handleMovement(dt);
    regenEnergy(dt);
    tickCooldowns(dt);
    m_animation.update(dt);
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}

void Player::takeDamage(float amount)
{
    m_health -= amount;
    if (m_health < 0.f) m_health = 0.f;
}

void Player::handleMovement(float dt)
{
    sf::Vector2f vel(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) vel.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) vel.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) vel.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) vel.x += 1.f;

    float len = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (len > 0.f)
    {
        vel /= len;
        m_sprite.move(vel * m_speed * dt);
    }

    sf::Vector2f pos = m_sprite.getPosition();
    pos.x = std::max(20.f, std::min(pos.x, 1260.f));
    pos.y = std::max(20.f, std::min(pos.y, 700.f));
    m_sprite.setPosition(pos);
}

void Player::regenEnergy(float dt)
{
    m_energy += 8.f * dt;
    if (m_energy > PLAYER_MAX_ENERGY)
        m_energy = PLAYER_MAX_ENERGY;
}

void Player::tickCooldowns(float dt)
{
    if (m_primaryCooldown   > 0.f) m_primaryCooldown   -= dt;
    if (m_specialCooldown   > 0.f) m_specialCooldown   -= dt;
    if (m_secondaryCooldown > 0.f) m_secondaryCooldown -= dt;
}

bool Player::spendEnergy(float amount)
{
    if (m_energy < amount) return false;
    m_energy -= amount;
    return true;
}

void Player::faceToward(sf::Vector2f target)
{
    sf::Vector2f pos = m_sprite.getPosition();
    if (target.x < pos.x)
        m_sprite.setScale(-1.f, 1.f);   // face left
    else
        m_sprite.setScale(1.f, 1.f);    // face right
}
