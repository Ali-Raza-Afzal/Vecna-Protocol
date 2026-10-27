// ============================================================
// Game.cpp — Master game loop
//
// Implements the core loop:
//   processEvents → update → render
//
// Also owns the TAB character switch and protocol advancement.
// ============================================================

#include "Game.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "UIManager.h"
#include "ProtocolManager.h"
#include "CollisionManager.h"
#include "AliPlayer.h"
#include "AhaanPlayer.h"

#include <SFML/Window/Keyboard.hpp>
#include <iostream>

// ── Constructor ──────────────────────────────────────────────
Game::Game()
    : m_window(sf::VideoMode(1280, 720), "UPSIDE DOWN: VECNA PROTOCOL",
               sf::Style::Fullscreen)
{
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);

    initManagers();
    initPlayers();
    initCinematicOverlay();

    std::cout << "[Game] Initialised OK\n";
}

// ── Public run loop ──────────────────────────────────────────
void Game::run()
{
    while (m_window.isOpen())
    {
        m_deltaTime = m_clock.restart().asSeconds();
        // Cap delta so a freeze doesn't cause physics explosion
        if (m_deltaTime > 0.05f) m_deltaTime = 0.05f;

        processEvents();
        update();
        render();
    }
}

// ── Events ───────────────────────────────────────────────────
void Game::processEvents()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        // Window closed
        if (event.type == sf::Event::Closed)
            m_window.close();

        // Keyboard events
        if (event.type == sf::Event::KeyPressed)
        {
            // ESC — pause / unpause
            if (event.key.code == sf::Keyboard::Escape)
            {
                if (m_state == GameState::Playing)
                    m_state = GameState::Paused;
                else if (m_state == GameState::Paused)
                    m_state = GameState::Playing;
            }

            // TAB — switch character
            if (event.key.code == sf::Keyboard::Tab &&
                m_state == GameState::Playing)
            {
                switchCharacter();
            }

            // Enter on menus
            if (event.key.code == sf::Keyboard::Return)
            {
                if (m_state == GameState::MainMenu)
                {
                    m_state = GameState::Cutscene;
                    m_scene->startIntro();
                }
                else if (m_state == GameState::GameOver ||
                         m_state == GameState::Victory)
                {
                    m_state = GameState::MainMenu;
                    m_scene->loadMenu();
                }
            }
        }

        // Pass mouse clicks to active player (attacks)
        if (m_state == GameState::Playing && m_activePlayer)
        {
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2f mousePos =
                    m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

                if (event.mouseButton.button == sf::Mouse::Left)
                    m_activePlayer->primaryAttack(mousePos);

                if (event.mouseButton.button == sf::Mouse::Right)
                    m_activePlayer->specialAbility(mousePos);
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Q)
            {
                sf::Vector2f mousePos =
                    m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
                m_activePlayer->secondaryAbility(mousePos);
            }
        }
    }
}

// ── Update ───────────────────────────────────────────────────
void Game::update()
{
    handleFade(m_deltaTime);

    switch (m_state)
    {
    case GameState::MainMenu:
        m_scene->updateMenu(m_deltaTime);
        break;

    case GameState::Cutscene:
        m_scene->updateCutscene(m_deltaTime);
        // Scene tells us when cutscene ends
        if (m_scene->isCutsceneDone())
        {
            m_state = GameState::Playing;
            m_fadingIn = true;
            m_fadeAlpha = 255.f;
        }
        break;

    case GameState::Playing:
        // Update active player
        if (m_activePlayer)
            m_activePlayer->update(m_deltaTime);

        // Update current scene (enemies, map, particles)
        m_scene->updateGameplay(m_deltaTime, m_activePlayer);

        // Collision detection
        m_collision->update(m_activePlayer,
                            m_scene->getEnemies(),
                            m_scene->getProjectiles());

        // Protocol timer / wave logic
        m_protocol->update(m_deltaTime);
        if (m_protocol->isTimerExpired())
        {
            m_state = GameState::GameOver;
        }

        // Check if current protocol is complete
        checkProtocolAdvance();

        // Sound reacts to gameplay context
        m_sound->update(m_deltaTime, m_scene->isInCombat());
        break;

    case GameState::Paused:
        // Nothing updates while paused
        break;

    case GameState::GameOver:
    case GameState::Victory:
        // Show end screen — wait for Enter
        break;
    }
}

// ── Render ───────────────────────────────────────────────────
void Game::render()
{
    m_window.clear(sf::Color(5, 5, 10)); // near-black base

    switch (m_state)
    {
    case GameState::MainMenu:
        m_scene->drawMenu(m_window);
        break;

    case GameState::Cutscene:
        m_scene->drawCutscene(m_window);
        break;

    case GameState::Playing:
        m_scene->drawGameplay(m_window);
        if (m_activePlayer)
            m_activePlayer->draw(m_window);
        m_ui->draw(m_window, m_activePlayer, m_protocol.get());
        break;

    case GameState::Paused:
        m_scene->drawGameplay(m_window);  // show blurred game behind
        m_ui->drawPauseMenu(m_window);
        break;

    case GameState::GameOver:
        m_ui->drawGameOver(m_window);
        break;

    case GameState::Victory:
        m_ui->drawVictory(m_window);
        break;
    }

    // Draw cinematic fade overlay on top of everything
    if (m_fadeAlpha > 0.f)
    {
        m_fadeRect.setFillColor(sf::Color(0, 0, 0,
            static_cast<sf::Uint8>(m_fadeAlpha)));
        m_window.draw(m_fadeRect);
    }

    m_window.display();
}

// ── Helpers ──────────────────────────────────────────────────

void Game::handleFade(float dt)
{
    // Fade in: darken → clear
    // Fade out: clear → darken  (set m_fadingIn = false before triggering)
    const float FADE_SPEED = 180.f; // alpha units per second

    if (m_fadingIn && m_fadeAlpha > 0.f)
    {
        m_fadeAlpha -= FADE_SPEED * dt;
        if (m_fadeAlpha < 0.f) m_fadeAlpha = 0.f;
    }
    else if (!m_fadingIn && m_fadeAlpha < 255.f)
    {
        m_fadeAlpha += FADE_SPEED * dt;
        if (m_fadeAlpha > 255.f) m_fadeAlpha = 255.f;
    }
}

void Game::switchCharacter()
{
    // Toggle between Ali and Ahaan
    if (m_activePlayer == m_ali.get())
    {
        m_activePlayer = m_ahaan.get();
        m_sound->playSFX("switch");
        m_ui->showCharacterBanner("AHAAN SHUJA — Illusion Master");
    }
    else
    {
        m_activePlayer = m_ali.get();
        m_sound->playSFX("switch");
        m_ui->showCharacterBanner("ALI RAZA — Telekinetic Warrior");
    }
}

void Game::checkProtocolAdvance()
{
    // Stop if protocol not complete
    if (!m_scene->isProtocolComplete())
        return;

    std::cout << "[Game] Protocol Complete!\n";

    int next = m_protocol->getCurrentProtocol() + 1;

    // Finished Protocol 3 -> Victory
    if (next > 3)
    {
        m_state = GameState::Victory;
        m_sound->playMusic("victory");
        return;
    }

    // Advance protocol
    m_protocol->advanceProtocol();

    // Load next protocol scene
    m_scene->loadProtocol(
        m_protocol->getCurrentProtocol()
    );

    // Optional fade-in effect
    m_fadingIn = true;
    m_fadeAlpha = 255.f;
}

void Game::initManagers()
{
    m_scene     = std::make_unique<SceneManager>();
    m_sound     = std::make_unique<SoundManager>();
    m_ui        = std::make_unique<UIManager>();
    m_protocol  = std::make_unique<ProtocolManager>();
    m_collision = std::make_unique<CollisionManager>();

    m_scene->loadMenu();
    m_sound->playMusic("menu_ambient");
}

void Game::initPlayers()
{
    m_ali   = std::make_unique<AliPlayer>(sf::Vector2f(400.f, 360.f));
    m_ahaan = std::make_unique<AhaanPlayer>(sf::Vector2f(450.f, 360.f));
    m_activePlayer = m_ali.get();   // Ali starts active
}

void Game::initCinematicOverlay()
{
    m_fadeRect.setSize(sf::Vector2f(1280.f, 720.f));
    m_fadeRect.setPosition(0.f, 0.f);
    m_fadeRect.setFillColor(sf::Color(0, 0, 0, 255));
    m_fadingIn = true;
    m_fadeAlpha = 255.f;
}