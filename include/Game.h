// ============================================================
// Game.h — Master game controller
//
// Game owns the SFML window, the game loop, and all managers.
// Everything starts and ends here.
// ============================================================
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include "AliPlayer.h"
#include "AhaanPlayer.h"
#include "CollisionManager.h"

#include "SceneManager.h"
#include "SoundManager.h"
#include "UIManager.h"
#include "ProtocolManager.h"

// Forward declarations (keeps compile times fast)

class Player;


// ── Game states ──────────────────────────────────────────────
enum class GameState
{
    MainMenu,
    Cutscene,   // story text / cinematic bars
    Playing,
    Paused,
    GameOver,
    Victory
};

// ============================================================
class Game
{
public:
    Game();
    void run();   // ← the public entry point

private:
    // Core SFML objects
    sf::RenderWindow m_window;
    sf::Clock        m_clock;        // tracks frame delta-time
    float            m_deltaTime{0.f};

    // Current state
    GameState m_state{GameState::MainMenu};

    // All managers (owned via unique_ptr for clean lifetime)
    std::unique_ptr<SceneManager>    m_scene;
    std::unique_ptr<SoundManager>    m_sound;
    std::unique_ptr<UIManager>       m_ui;
    std::unique_ptr<ProtocolManager> m_protocol;
    std::unique_ptr<CollisionManager> m_collision;

    // Players — only one is "active" at a time
    std::unique_ptr<AliPlayer>   m_ali;
    std::unique_ptr<AhaanPlayer> m_ahaan;
    Player* m_activePlayer{nullptr};  // raw pointer, no ownership

    // Cinematic overlay
    sf::RectangleShape m_fadeRect;   // black fill, alpha varies
    float m_fadeAlpha{255.f};        // 255 = black, 0 = clear
    bool  m_fadingIn{true};

    // ── Private helpers ──────────────────────────────────────
    void processEvents();
    void update();
    void render();

    void handleFade(float dt);        // fades in/out between scenes
    void switchCharacter();           // TAB logic
    void checkProtocolAdvance();      // did we beat the current protocol?

    void initManagers();              // called once in constructor
    void initPlayers();
    void initCinematicOverlay();
};