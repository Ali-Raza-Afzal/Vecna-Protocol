// ============================================================
// SoundManager.cpp
//
// RECOMMENDED FREE AUDIO SOURCES:
//   freesound.org       — SFX (CC0 / CC-BY)
//   opengameart.org     — Music + SFX (various open licenses)
//   incompetech.com     — Kevin MacLeod's royalty-free music
//   pixabay.com/music   — No-attribution required
//
// SUGGESTED TRACKS:
//   menu_ambient.ogg    — Slow dark drone (Pixabay: "Dark Ambient")
//   explore.ogg         — Tense low pad (Kevin MacLeod: "Ossuary 5")
//   combat.ogg          — Fast dark music (Kevin MacLeod: "Darkest Child")
//   boss.ogg            — Intense orchestral (Kevin MacLeod: "Final Count")
//   victory.ogg         — Emotional end theme
//
// SUGGESTED SFX:
//   switch.wav          — Whoosh for character switch
//   hurt.wav            — Pain grunt
//   shockwave.wav       — Deep bass boom
//   illusion.wav        — Ethereal sweep
//   levelup.wav         — Protocol transition chime
// ============================================================

#include "SoundManager.h"
#include <iostream>

// ── Constructor ──────────────────────────────────────────────
SoundManager::SoundManager()
{
    // Load all SFX at startup.
    // If a file is missing, the game still runs (just silently).
    loadSFX("switch",    "assets/sounds/switch.wav");
    loadSFX("hurt",      "assets/sounds/hurt.wav");
    loadSFX("shockwave", "assets/sounds/shockwave.wav");
    loadSFX("illusion",  "assets/sounds/illusion.wav");
    loadSFX("levelup",   "assets/sounds/levelup.wav");
    loadSFX("click",     "assets/sounds/click.wav");
}

// ── Music ────────────────────────────────────────────────────
void SoundManager::playMusic(const std::string& trackName)
{
    if (m_currentTrack == trackName) return;  // already playing

    m_currentTrack = trackName;
    std::string path = "assets/music/" + trackName + ".ogg";

    if (!m_music.openFromFile(path))
    {
        std::cerr << "[Sound] Music not found: " << path << "\n";
        return;
    }

    m_music.setLoop(true);
    m_music.setVolume(m_currentVolume);
    m_music.play();
    std::cout << "[Sound] Playing: " << path << "\n";
}

void SoundManager::stopMusic()
{
    m_music.stop();
    m_currentTrack = "";
}

void SoundManager::setMusicVolume(float vol)
{
    m_targetVolume = vol;
}

// ── SFX ──────────────────────────────────────────────────────
void SoundManager::playSFX(const std::string& sfxName)
{
    auto it = m_sounds.find(sfxName);
    if (it != m_sounds.end())
    {
        it->second.setVolume(80.f);
        it->second.play();
    }
    else
    {
        std::cerr << "[Sound] SFX not loaded: " << sfxName << "\n";
    }
}

// ── Update (cross-fade + combat music swap) ──────────────────
void SoundManager::update(float dt, bool isInCombat)
{
    // Smoothly fade music volume toward target
    const float FADE_SPEED = 30.f;
    if (m_currentVolume < m_targetVolume)
        m_currentVolume = std::min(m_currentVolume + FADE_SPEED * dt, m_targetVolume);
    else if (m_currentVolume > m_targetVolume)
        m_currentVolume = std::max(m_currentVolume - FADE_SPEED * dt, m_targetVolume);

    m_music.setVolume(m_currentVolume);
}

// ── Private helpers ───────────────────────────────────────────
void SoundManager::loadSFX(const std::string& name,
                             const std::string& filePath)
{
    sf::SoundBuffer buf;
    if (!buf.loadFromFile(filePath))
    {
        std::cerr << "[Sound] SFX not found: " << filePath << "\n";
        return;
    }
    m_buffers[name] = std::move(buf);
    m_sounds[name].setBuffer(m_buffers[name]);
}