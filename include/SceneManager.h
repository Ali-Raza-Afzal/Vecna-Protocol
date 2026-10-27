// ============================================================
// SceneManager.h — All game scenes / protocols
//
// This is the scene director. It knows:
//   - Which protocol map to draw
//   - Which enemies to spawn
//   - When a protocol is "complete"
//   - How to do cutscene dialogue
//
// The scene draws the tilemap background, manages the enemy list,
// and handles any protocol-specific events (hive node destruction,
// fake wall reveals, boss trigger).
// ============================================================
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>

class Player;
class Enemy;
class Projectile;

// ── Fake wall (Protocol 1 mechanic) ──────────────────────────
struct FakeWall
{
    sf::RectangleShape shape;
    bool hidden{false};   // hidden = revealed by Ahaan's power
};

// ============================================================
class SceneManager
{
public:
    SceneManager();

    // ── Scene loading ─────────────────────────────────────────
    void loadMenu();
    void loadProtocol(int protocol);
    void startIntro();     // kicks off the opening cutscene

    // ── Update ────────────────────────────────────────────────
    void updateMenu(float dt);
    void updateCutscene(float dt);
    void updateGameplay(float dt, Player* player);

    // ── Draw ──────────────────────────────────────────────────
    void drawMenu(sf::RenderWindow& window);
    void drawCutscene(sf::RenderWindow& window);
    void drawGameplay(sf::RenderWindow& window);

    // ── Queries ───────────────────────────────────────────────
    bool isCutsceneDone()    const { return m_cutsceneDone; }
    bool isProtocolComplete()const { return m_protocolComplete; }
    bool isInCombat()        const { return m_inCombat; }

    // Getters for CollisionManager
    std::vector<std::shared_ptr<Enemy>>& getEnemies()     { return m_enemies; }
    std::vector<Projectile>&             getProjectiles() { return m_enemyProjectiles; }

    // Protocol 1 — Ahaan can reveal fake walls
    void revealFakeWalls();   // called when isRevealActive() is true

private:
    int m_currentProtocol{0};
    bool m_cutsceneDone{false};
    bool m_protocolComplete{false};
    bool m_inCombat{false};

    // ── Backgrounds ───────────────────────────────────────────
    sf::Texture m_bgTexture;
    sf::Sprite  m_bgSprite;

    // ── Enemies ───────────────────────────────────────────────
    std::vector<std::shared_ptr<Enemy>> m_enemies;
    std::vector<Projectile>             m_enemyProjectiles;

    // ── Protocol 1 ────────────────────────────────────────────
    std::vector<FakeWall> m_fakeWalls;
    int m_hiveNodesRemaining{0};   // Protocol 2

    // ── Atmosphere ────────────────────────────────────────────
    sf::RectangleShape m_fogOverlay;  // darkens the scene
    float m_fogAlpha{0.f};
    float m_fogFlicker{0.f};

    // ── Cutscene ──────────────────────────────────────────────
    struct DialogueLine { std::string speaker; std::string text; };
    std::vector<DialogueLine> m_cutsceneLines;
    int   m_cutsceneIndex{0};
    float m_cutsceneTimer{0.f};

    // ── Menu ──────────────────────────────────────────────────
    sf::Text m_titleText;
    sf::Text m_promptText;
    sf::Font m_font;
    bool     m_fontLoaded{false};
    float    m_menuFlicker{0.f};

    // ── Private helpers ───────────────────────────────────────
    void spawnProtocol0Enemies();
    void spawnProtocol1Enemies();
    void spawnProtocol2Enemies();
    void spawnProtocol3Boss();

    void setupProtocol1Walls();
    void updateFog(float dt);
    void drawFog(sf::RenderWindow& window);
    void collectEnemyProjectiles();
    void checkProtocolCompletion();

    bool loadBackground(const std::string& path);
};