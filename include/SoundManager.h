// ============================================================
// SoundManager.h — Music and SFX management
//
// Loads music tracks and sound effects.
// Music cross-fades between states (menu, explore, combat, boss).
// SFX are short sounds played once on demand.
//
// All assets should be royalty-free (see recommendations below).
// ============================================================
#pragma once

#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>

class SoundManager
{
public:
    SoundManager();

    // ── Music control ─────────────────────────────────────────
    void playMusic(const std::string& trackName);
    void stopMusic();
    void setMusicVolume(float vol);   // 0–100

    // ── SFX control ───────────────────────────────────────────
    void playSFX(const std::string& sfxName);

    // Called from Game::update() — handles cross-fade logic
    void update(float dt, bool isInCombat);

private:
    // Music uses sf::Music (streams from disk — big files)
    sf::Music m_music;
    std::string m_currentTrack;

    // SFX use sf::Sound + sf::SoundBuffer (loaded into RAM — small files)
    std::map<std::string, sf::SoundBuffer> m_buffers;
    std::map<std::string, sf::Sound>       m_sounds;

    float m_targetVolume{60.f};
    float m_currentVolume{0.f};

    void loadSFX(const std::string& name, const std::string& filePath);
    void tryLoadMusic(const std::string& filePath);
};