// ============================================================
// UIManager.cpp — HUD rendering
// ============================================================

#include "UIManager.h"
#include "Player.h"
#include "ProtocolManager.h"
#include <sstream>
#include <iomanip>
#include <iostream>

// ── Constructor ──────────────────────────────────────────────
UIManager::UIManager()
{
    // Load font — try a common system path
    // IMPORTANT: download a free pixel/horror font and place at this path.
    // Recommended: "Press Start 2P" (Google Fonts, OFL license)
    if (!m_font.loadFromFile("assets/fonts/main.ttf"))
    {
        std::cerr << "[UI] Font not found — text will be invisible.\n"
                  << "     Download a free font and place at: assets/fonts/main.ttf\n";
        m_fontLoaded = false;
    }
    else
    {
        m_fontLoaded = true;
    }

    // Cinematic bars (black, top and bottom)
    m_topBar.setSize({1280.f, 60.f});
    m_topBar.setFillColor(sf::Color(0, 0, 0, 210));
    m_topBar.setPosition(0.f, 0.f);

    m_bottomBar.setSize({1280.f, 60.f});
    m_bottomBar.setFillColor(sf::Color(0, 0, 0, 210));
    m_bottomBar.setPosition(0.f, 660.f);

    // Dialogue box
    m_dialogueBox.setSize({1100.f, 100.f});
    m_dialogueBox.setPosition(90.f, 590.f);
    m_dialogueBox.setFillColor(sf::Color(5, 5, 20, 230));
    m_dialogueBox.setOutlineColor(sf::Color(80, 0, 120, 180));
    m_dialogueBox.setOutlineThickness(2.f);
}

// ── Main HUD ─────────────────────────────────────────────────
void UIManager::draw(sf::RenderWindow& window, Player* player,
                      ProtocolManager* protocol)
{
    window.draw(m_topBar);
    window.draw(m_bottomBar);

    if (!player || !protocol) return;

    // Health bar — top left
    drawBar(window, {20.f, 20.f}, {200.f, 18.f},
            player->getHealth(), player->getMaxHealth(),
            sf::Color(200, 30, 30));

    // Energy bar — below health
    drawBar(window, {20.f, 46.f}, {200.f, 14.f},
            player->getEnergy(), player->getMaxEnergy(),
            sf::Color(60, 120, 255));

    // Labels
    drawText(window, "HP", {226.f, 22.f}, 14, sf::Color(255, 100, 100));
    drawText(window, "EP", {226.f, 46.f}, 14, sf::Color(100, 160, 255));

    // Character name — top center
    drawText(window, player->getName(),
             {540.f, 18.f}, 16, sf::Color(220, 200, 255));

    // Protocol indicator — top right
    std::string proto = "PROTOCOL " + std::to_string(protocol->getCurrentProtocol());
    drawText(window, proto, {980.f, 18.f}, 14, sf::Color(180, 60, 255));

    // Timer countdown — top right, below protocol
    std::string timerStr = "TIME: " + formatTime(protocol->getRemainingTime());
    sf::Color timerColor = protocol->getRemainingTime() < 120.f
        ? sf::Color(255, 80, 80)    // red when < 2 minutes
        : sf::Color(255, 220, 100);
    drawText(window, timerStr, {980.f, 38.f}, 14, timerColor);

    // Controls hint — bottom bar
    drawText(window, "[TAB] Switch   [LMB] Attack   [RMB] Special   [Q] Ability   [ESC] Pause",
             {200.f, 666.f}, 12, sf::Color(140, 140, 160));

    // Character banner (fades in/out on TAB)
    updateBanner(0.f);  // dt passed separately; this just draws
    if (m_bannerTimer > 0.f)
    {
        float alpha = std::min(m_bannerTimer / m_bannerDuration, 1.f) * 255.f;
        m_bannerText.setFillColor(sf::Color(220, 180, 255,
            static_cast<sf::Uint8>(alpha)));
        window.draw(m_bannerText);
    }
}

// ── Pause menu ───────────────────────────────────────────────
void UIManager::drawPauseMenu(sf::RenderWindow& window)
{
    // Semi-transparent dark overlay
    sf::RectangleShape overlay({1280.f, 720.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    drawText(window, "PAUSED",         {500.f, 250.f}, 48, sf::Color(220, 180, 255));
    drawText(window, "Press ESC to resume", {440.f, 340.f}, 20, sf::Color(180, 180, 200));
    drawText(window, "Press Q to quit (not mapped yet)", {400.f, 380.f}, 16, sf::Color(120, 120, 140));
}

// ── Game over ─────────────────────────────────────────────────
void UIManager::drawGameOver(sf::RenderWindow& window)
{
    sf::RectangleShape overlay({1280.f, 720.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    drawText(window, "THEY ARE GONE.",          {400.f, 220.f}, 40, sf::Color(200, 30, 30));
    drawText(window, "Abdul and Shaan did not survive.", {330.f, 300.f}, 22, sf::Color(200, 150, 150));
    drawText(window, "Vecna wins.",              {500.f, 360.f}, 26, sf::Color(180, 50, 50));
    drawText(window, "Press ENTER to return to menu.",  {360.f, 480.f}, 18, sf::Color(140, 140, 160));
}

// ── Victory ──────────────────────────────────────────────────
void UIManager::drawVictory(sf::RenderWindow& window)
{
    sf::RectangleShape overlay({1280.f, 720.f});
    overlay.setFillColor(sf::Color(5, 0, 20, 200));
    window.draw(overlay);

    drawText(window, "VECNA FALLS.",                       {400.f, 180.f}, 44, sf::Color(180, 80, 255));
    drawText(window, "Abdul Nafhay and Shaan Sikandar live.", {290.f, 270.f}, 20, sf::Color(200, 180, 255));
    drawText(window, "Ali immobilised him.",               {460.f, 320.f}, 18, sf::Color(180, 180, 220));
    drawText(window, "Ahaan trapped him in his own illusions.", {330.f, 356.f}, 18, sf::Color(180, 180, 220));
    drawText(window, "Vecna could no longer tell what was real.", {310.f, 392.f}, 18, sf::Color(150, 150, 190));
    drawText(window, "It ended four seconds before midnight.",   {340.f, 450.f}, 18, sf::Color(255, 200, 100));
    drawText(window, "Press ENTER to continue.",           {430.f, 540.f}, 16, sf::Color(140, 140, 160));
}

// ── Character banner ─────────────────────────────────────────
void UIManager::showCharacterBanner(const std::string& text)
{
    if (!m_fontLoaded) return;

    m_bannerText.setFont(m_font);
    m_bannerText.setString(text);
    m_bannerText.setCharacterSize(24);
    m_bannerText.setPosition(
        640.f - m_bannerText.getGlobalBounds().width / 2.f,
        340.f);
    m_bannerTimer = m_bannerDuration;
}

void UIManager::updateBanner(float dt)
{
    if (m_bannerTimer > 0.f)
        m_bannerTimer -= dt;
    if (m_bannerTimer < 0.f)
        m_bannerTimer = 0.f;
}

// ── Typewriter dialogue ───────────────────────────────────────
void UIManager::startDialogue(const std::string& text, float charsPerSecond)
{
    m_fullDialogue    = text;
    m_visibleDialogue = "";
    m_charsPerSecond  = charsPerSecond;
    m_charsAccum      = 0.f;
}

void UIManager::updateDialogue(float dt)
{
    if (m_visibleDialogue.size() >= m_fullDialogue.size()) return;

    m_charsAccum += m_charsPerSecond * dt;
    while (m_charsAccum >= 1.f &&
           m_visibleDialogue.size() < m_fullDialogue.size())
    {
        m_visibleDialogue += m_fullDialogue[m_visibleDialogue.size()];
        m_charsAccum -= 1.f;
    }
}

void UIManager::drawDialogue(sf::RenderWindow& window)
{
    window.draw(m_dialogueBox);
    drawText(window, m_visibleDialogue, {110.f, 605.f}, 14,
             sf::Color(220, 210, 255));
}

bool UIManager::isDialogueDone() const
{
    return m_visibleDialogue.size() >= m_fullDialogue.size();
}

// ── Private helpers ───────────────────────────────────────────

void UIManager::drawBar(sf::RenderWindow& window,
                         sf::Vector2f pos, sf::Vector2f size,
                         float value, float maxValue, sf::Color fillColor)
{
    m_barBg.setSize(size);
    m_barBg.setPosition(pos);
    m_barBg.setFillColor(sf::Color(30, 10, 30));
    window.draw(m_barBg);

    float pct = (maxValue > 0.f) ? (value / maxValue) : 0.f;
    m_barFill.setSize({size.x * pct, size.y});
    m_barFill.setPosition(pos);
    m_barFill.setFillColor(fillColor);
    window.draw(m_barFill);
}

void UIManager::drawText(sf::RenderWindow& window, const std::string& str,
                          sf::Vector2f pos, unsigned int size, sf::Color color)
{
    if (!m_fontLoaded) return;

    sf::Text text;
    text.setFont(m_font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(pos);
    window.draw(text);
}

std::string UIManager::formatTime(float seconds) const
{
    int mins = static_cast<int>(seconds) / 60;
    int secs = static_cast<int>(seconds) % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << mins
        << ":" << std::setw(2) << std::setfill('0') << secs;
    return oss.str();
}