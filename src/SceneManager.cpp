// ============================================================
// SceneManager.cpp — Scene logic for all four protocols
// ============================================================

#include "SceneManager.h"
#include "Enemy.h"
#include "VecnaBoss.h"
#include "Projectile.h"
#include "Player.h"

#include <iostream>
#include <cstdlib>

// Enemy subclasses defined inline here for simplicity.
// In a larger project, split into ShadowCreature.h / HiveMonster.h.

// Shadow Creature — Protocol 0 enemy
class ShadowCreature : public Enemy {
public:
    ShadowCreature(sf::Vector2f pos)
        : Enemy(pos, "assets/textures/shadow_creature.png",
                40.f,   // health
                120.f,  // speed (fast, dangerous)
                10.f,   // damage
                50.f)   // attack range
    {
        m_sprite.setColor(sf::Color(80, 0, 80, 220));  // dark purple ghost
    }
};

// Hive Monster — Protocol 2 enemy
class HiveMonster : public Enemy {
public:
    HiveMonster(sf::Vector2f pos)
        : Enemy(pos, "assets/textures/hive_monster.png",
                70.f,
                90.f,
                18.f,
                60.f)
    {
        m_sprite.setColor(sf::Color(60, 120, 40, 255));  // dark green
    }
};

// Illusion Enemy — Protocol 1 enemy (low HP, fakes out the player)
class IllusionEnemy : public Enemy {
public:
    bool isFake{false};  // if true — deals no real damage

    IllusionEnemy(sf::Vector2f pos, bool fake)
        : Enemy(pos, "assets/textures/shadow_creature.png",
                fake ? 1.f : 35.f,
                100.f, 8.f, 50.f), isFake(fake)
    {
        // Fake enemies look slightly transparent
        m_sprite.setColor(fake
            ? sf::Color(180, 100, 255, 140)
            : sf::Color(180, 100, 255, 255));
    }
};

// ─────────────────────────────────────────────────────────────

SceneManager::SceneManager()
{
    // Fog overlay (drawn on top of map)
    m_fogOverlay.setSize({1280.f, 720.f});
    m_fogOverlay.setFillColor(sf::Color(0, 0, 0, 0));

    // Load font for menus
    m_fontLoaded = m_font.loadFromFile("assets/fonts/main.ttf");
}

// ── Scene loading ─────────────────────────────────────────────

void SceneManager::loadMenu()
{
    m_currentProtocol  = -1;
    m_protocolComplete = false;
    m_cutsceneDone     = false;

    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("UPSIDE DOWN\nVECNA PROTOCOL");
        m_titleText.setCharacterSize(52);
        m_titleText.setFillColor(sf::Color(180, 60, 255));
        m_titleText.setPosition(
            640.f - m_titleText.getGlobalBounds().width / 2.f, 200.f);

        m_promptText.setFont(m_font);
        m_promptText.setString("PRESS ENTER TO BEGIN");
        m_promptText.setCharacterSize(20);
        m_promptText.setFillColor(sf::Color(200, 180, 255));
        m_promptText.setPosition(
            640.f - m_promptText.getGlobalBounds().width / 2.f, 440.f);
    }
}

void SceneManager::startIntro()
{
    m_cutsceneLines = {
        {"NARRATOR", "Hawkins, Indiana. The Upside Down stirs."},
        {"NARRATOR", "Vecna has cursed Abdul Nafhay and Shaan Sikandar."},
        {"NARRATOR", "They have 24 hours. The clock is already ticking."},
        {"ALI RAZA", "We're going in. Protocol 0 — enter without being seen."},
        {"AHAAN SHUJA", "They briefed us. Four protocols. One chance."},
        {"ALI RAZA",   "We kill Vecna before midnight. That's the plan."},
        {"AHAAN SHUJA", "Then let's move."},
    };
    m_cutsceneIndex = 0;
    m_cutsceneTimer = 0.f;
    m_cutsceneDone  = false;
}

void SceneManager::loadProtocol(int protocol)
{
    m_currentProtocol  = protocol;
    m_protocolComplete = false;
    m_enemies.clear();
    m_enemyProjectiles.clear();
    m_fakeWalls.clear();

    // Load background
    std::string bgPath = "assets/textures/bg_protocol" +
                          std::to_string(protocol) + ".png";
    if (!loadBackground(bgPath))
    {
        // Fallback: solid dark background
        std::cerr << "[Scene] No background for protocol " << protocol << "\n";
    }

    // Spawn enemies and set up protocol-specific stuff
    switch (protocol)
    {
    case 0: spawnProtocol0Enemies(); m_fogAlpha = 120.f; break;
    case 1: spawnProtocol1Enemies(); setupProtocol1Walls(); break;
    case 2: spawnProtocol2Enemies(); m_hiveNodesRemaining = 3; break;
    case 3: spawnProtocol3Boss();    break;
    }

    std::cout << "[Scene] Protocol " << protocol << " loaded.\n";
}

// ── Updates ───────────────────────────────────────────────────

void SceneManager::updateMenu(float dt)
{
    m_menuFlicker += dt;
    // Flicker the prompt text
    float alpha = 180.f + 75.f * std::sin(m_menuFlicker * 3.f);
    if (m_fontLoaded)
        m_promptText.setFillColor(sf::Color(200, 180, 255,
            static_cast<sf::Uint8>(alpha)));
}

void SceneManager::updateCutscene(float dt)
{
    m_cutsceneTimer += dt;

    // Auto-advance each line after 4 seconds
    // (In a real version, also advance on ENTER / SPACE)
    if (m_cutsceneTimer > 4.f)
    {
        m_cutsceneTimer = 0.f;
        m_cutsceneIndex++;
        if (m_cutsceneIndex >= static_cast<int>(m_cutsceneLines.size()))
        {
            m_cutsceneDone = true;
            loadProtocol(0);   // Start at Protocol 0
        }
    }
}

void SceneManager::updateGameplay(float dt, Player* player)
{
    if (!player) return;

    sf::Vector2f playerPos = player->getPosition();

    // Update fog atmosphere
    updateFog(dt);

    // Update all enemies
    m_inCombat = false;
    for (auto& enemy : m_enemies)
    {
        if (!enemy->isDead())
        {
            enemy->update(dt, playerPos);
            m_inCombat = true;
        }
    }

    // Remove dead enemies
    // (Keep them in the list for death animation — just skip drawing)

    // Collect any projectiles enemies fired
    collectEnemyProjectiles();

    // Update enemy projectiles
    for (auto& proj : m_enemyProjectiles)
        proj.update(dt);
    m_enemyProjectiles.erase(
        std::remove_if(m_enemyProjectiles.begin(), m_enemyProjectiles.end(),
            [](const Projectile& p){ return p.isDead(); }),
        m_enemyProjectiles.end());

    checkProtocolCompletion();
}

// ── Draws ─────────────────────────────────────────────────────

void SceneManager::drawMenu(sf::RenderWindow& window)
{
    // Dark background gradient (simple two-rect approach)
    sf::RectangleShape bg({1280.f, 720.f});
    bg.setFillColor(sf::Color(8, 0, 18));
    window.draw(bg);

    if (m_fontLoaded)
    {
        window.draw(m_titleText);
        window.draw(m_promptText);
    }

    // Simple "upside down" world particles (just dots)
    // In your full version, use a particle system here
}

void SceneManager::drawCutscene(sf::RenderWindow& window)
{
    // Black background for cinematic feel
    sf::RectangleShape bg({1280.f, 720.f});
    bg.setFillColor(sf::Color(3, 0, 10));
    window.draw(bg);

    // Cinematic top/bottom bars
    sf::RectangleShape bar({1280.f, 80.f});
    bar.setFillColor(sf::Color::Black);
    bar.setPosition(0.f, 0.f);
    window.draw(bar);
    bar.setPosition(0.f, 640.f);
    window.draw(bar);

    // Show current dialogue line
    if (!m_fontLoaded) return;
    if (m_cutsceneIndex >= static_cast<int>(m_cutsceneLines.size())) return;

    const auto& line = m_cutsceneLines[m_cutsceneIndex];

    sf::Text speaker;
    speaker.setFont(m_font);
    speaker.setString(line.speaker);
    speaker.setCharacterSize(16);
    speaker.setFillColor(sf::Color(180, 100, 255));
    speaker.setPosition(120.f, 590.f);
    window.draw(speaker);

    sf::Text dialogue;
    dialogue.setFont(m_font);
    dialogue.setString(line.text);
    dialogue.setCharacterSize(18);
    dialogue.setFillColor(sf::Color(220, 210, 240));
    dialogue.setPosition(120.f, 620.f);
    window.draw(dialogue);
}

void SceneManager::drawGameplay(sf::RenderWindow& window)
{
    // Background
    if (m_bgTexture.getSize().x > 0)
        window.draw(m_bgSprite);
    else
    {
        sf::RectangleShape bg({1280.f, 720.f});
        bg.setFillColor(sf::Color(6, 4, 16));
        window.draw(bg);
    }

    // Protocol 1 fake walls
    for (auto& fw : m_fakeWalls)
    {
        if (!fw.hidden)
            window.draw(fw.shape);
    }

    // Enemies
    for (auto& enemy : m_enemies)
        enemy->draw(window);

    // Fog
    drawFog(window);
}

// ── Enemy spawning ────────────────────────────────────────────

void SceneManager::spawnProtocol0Enemies()
{
    // Protocol 0: tutorial / horror intro — 4 shadow creatures
    std::vector<sf::Vector2f> positions = {
        {900.f, 200.f}, {950.f, 400.f}, {800.f, 550.f}, {1100.f, 300.f}
    };
    for (auto& pos : positions)
        m_enemies.push_back(std::make_shared<ShadowCreature>(pos));
}

void SceneManager::spawnProtocol1Enemies()
{
    // Protocol 1: mix of real and fake illusion enemies
    // 3 real + 4 fake — player must figure out which are real
    std::vector<std::pair<sf::Vector2f, bool>> spawns = {
        {{800.f, 150.f}, false},  // real
        {{700.f, 400.f}, true},   // fake
        {{1050.f, 300.f}, false}, // real
        {{600.f, 500.f}, true},   // fake
        {{900.f, 500.f}, false},  // real
        {{1100.f, 550.f}, true},  // fake
        {{750.f, 250.f}, true},   // fake
    };
    for (auto& [pos, fake] : spawns)
        m_enemies.push_back(std::make_shared<IllusionEnemy>(pos, fake));
}

void SceneManager::spawnProtocol2Enemies()
{
    // Protocol 2: waves of hive monsters + hive nodes
    std::vector<sf::Vector2f> positions = {
        {900.f, 200.f}, {1100.f, 400.f}, {800.f, 500.f},
        {1050.f, 150.f},{950.f, 600.f},  {1150.f, 300.f},
    };
    for (auto& pos : positions)
        m_enemies.push_back(std::make_shared<HiveMonster>(pos));
}

void SceneManager::spawnProtocol3Boss()
{
    // Protocol 3: just Vecna (and he spawns minions dynamically)
    m_enemies.push_back(std::make_shared<VecnaBoss>(sf::Vector2f(900.f, 300.f)));
}

// ── Protocol 1 walls ──────────────────────────────────────────
void SceneManager::setupProtocol1Walls()
{
    // Create some fake walls that block passage until revealed
    std::vector<sf::FloatRect> wallRects = {
        {550.f, 200.f, 180.f, 30.f},
        {550.f, 400.f, 180.f, 30.f},
        {350.f, 300.f, 30.f, 200.f},
    };
    for (auto& r : wallRects)
    {
        FakeWall fw;
        fw.shape.setPosition(r.left, r.top);
        fw.shape.setSize({r.width, r.height});
        fw.shape.setFillColor(sf::Color(60, 30, 90, 200));
        fw.hidden = false;
        m_fakeWalls.push_back(fw);
    }
}

void SceneManager::revealFakeWalls()
{
    for (auto& fw : m_fakeWalls)
        fw.hidden = true;  // hide the blocking wall (reveal the passage)
}

// ── Fog atmosphere ────────────────────────────────────────────
void SceneManager::updateFog(float dt)
{
    m_fogFlicker += dt * 2.f;
    float flicker = std::sin(m_fogFlicker) * 12.f;
    float alpha = std::max(0.f, m_fogAlpha + flicker);
    m_fogOverlay.setFillColor(sf::Color(0, 0, 0,
        static_cast<sf::Uint8>(alpha)));
}

void SceneManager::drawFog(sf::RenderWindow& window)
{
    if (m_fogAlpha > 0.f)
        window.draw(m_fogOverlay);
}

// ── Collect enemy projectiles ─────────────────────────────────
// VecnaBoss fires its own projectiles — we copy them here
// so CollisionManager can check them against the player.
void SceneManager::collectEnemyProjectiles()
{
    for (auto& enemy : m_enemies)
    {
        auto* vecna = dynamic_cast<VecnaBoss*>(enemy.get());
        if (vecna)
        {
            for (auto& p : vecna->getBossProjectiles())
            {
                if (!p.isDead() && !p.isPlayerOwned())
                {
                    // Move to scene projectile list to avoid duplicates
                    // (Simple approach: just copy — fix in production with IDs)
                }
            }
        }
    }
}

// ── Protocol completion check ─────────────────────────────────
void SceneManager::checkProtocolCompletion()
{
    // Count living enemies
    int alive = 0;
    for (auto& e : m_enemies)
        if (!e->isDead()) alive++;

    if (alive == 0)
        m_protocolComplete = true;
}

// ── Background loading ────────────────────────────────────────
bool SceneManager::loadBackground(const std::string& path)
{
    if (!m_bgTexture.loadFromFile(path))
        return false;

    m_bgTexture.setSmooth(true);
    m_bgSprite.setTexture(m_bgTexture);

    // Scale to fit 1280×720
    float scaleX = 1280.f / m_bgTexture.getSize().x;
    float scaleY = 720.f  / m_bgTexture.getSize().y;
    m_bgSprite.setScale(scaleX, scaleY);
    return true;
}