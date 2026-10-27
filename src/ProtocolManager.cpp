// ============================================================
// ProtocolManager.cpp
// ============================================================
#include "ProtocolManager.h"
#include <iostream>

ProtocolManager::ProtocolManager(float totalTimeSeconds)
    : m_timer(totalTimeSeconds)
{}

void ProtocolManager::update(float dt)
{
    if (m_timer > 0.f)
    {
        m_timer -= dt;
        if (m_timer < 0.f) m_timer = 0.f;
    }
}

void ProtocolManager::advanceProtocol()
{
    if (m_protocol < 3)
    {
        m_protocol++;
        std::cout << "[Protocol] Advanced to Protocol " << m_protocol << "\n";
    }
}