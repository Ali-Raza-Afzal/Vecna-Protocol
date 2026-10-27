// ============================================================
// ProtocolManager.h — Timer and protocol stage tracking
//
// The 24-hour curse is simulated as an in-game countdown.
// We map 24h to a playable duration (e.g. 30 minutes total).
// Each protocol gets a share of that timer.
//
// Simple design: one global timer counts down from start.
// Protocols advance based on gameplay completion (boss dead,
// all waves cleared, etc.) — not time.
// ============================================================
#pragma once

class ProtocolManager
{
public:
    // totalTimeSeconds = the whole game's timer
    // (default: 1800s = 30 minutes — adjust for testing)
    explicit ProtocolManager(float totalTimeSeconds = 1800.f);

    void update(float dt);

    // State
    int   getCurrentProtocol()  const { return m_protocol; }
    float getRemainingTime()    const { return m_timer; }
    bool  isTimerExpired()      const { return m_timer <= 0.f; }

    // Called when a protocol is beaten
    void advanceProtocol();

    // For testing — set timer directly
    void setTimer(float seconds) { m_timer = seconds; }

private:
    int   m_protocol{0};   // 0–3
    float m_timer;         // seconds remaining
};