// ============================================================
// UIManager.h — HUD, menus, and text rendering
//
// Draws:
//   - Health bar (red)
//   - Energy / power bar (blue or purple)
//   - 24-hour countdown timer
//   - Current protocol indicator
//   - Active character name
//   - Character switch banner (fades out)
//   - Pause screen overlay
//   - Game over / victory screens
// ============================================================
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Player;
class ProtocolManager;

class UIManager
{
public:
    UIManager();

    // Main HUD — shown during gameplay
    void draw(sf::RenderWindow& window, Player* player,
              ProtocolManager* protocol);

    // Menus / overlays
    void drawPauseMenu(sf::RenderWindow& window);
    void drawGameOver(sf::RenderWindow& window);
    void drawVictory(sf::RenderWindow& window);

    // Short banner that fades in and out when you switch characters
    void showCharacterBanner(const std::string& text);
    void updateBanner(float dt);

    // Typewriter dialogue (for cutscenes)
    void startDialogue(const std::string& text, float charsPerSecond = 25.f);
    void updateDialogue(float dt);
    void drawDialogue(sf::RenderWindow& window);
    bool isDialogueDone() const;

private:
    sf::Font m_font;
    bool     m_fontLoaded{false};

    // ── Bars ──────────────────────────────────────────────────
    sf::RectangleShape m_barBg;
    sf::RectangleShape m_barFill;

    // ── Banner ────────────────────────────────────────────────
    sf::Text  m_bannerText;
    float     m_bannerTimer{0.f};    // > 0 = visible
    float     m_bannerDuration{2.f};

    // ── Dialogue box ─────────────────────────────────────────
    sf::RectangleShape m_dialogueBox;
    sf::Text           m_dialogueText;
    std::string        m_fullDialogue;
    std::string        m_visibleDialogue;
    float              m_dialogueTimer{0.f};
    float              m_charsPerSecond{25.f};
    float              m_charsAccum{0.f};

    // ── Cinematic bars (top and bottom black bars) ────────────
    sf::RectangleShape m_topBar;
    sf::RectangleShape m_bottomBar;

    // ── Helpers ───────────────────────────────────────────────
    void drawBar(sf::RenderWindow& window,
                 sf::Vector2f pos, sf::Vector2f size,
                 float value, float maxValue,
                 sf::Color fillColor);

    void drawText(sf::RenderWindow& window,
                  const std::string& str,
                  sf::Vector2f pos,
                  unsigned int size,
                  sf::Color color);

    std::string formatTime(float seconds) const;
};