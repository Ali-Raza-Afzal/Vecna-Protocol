// ============================================================
// Animation.cpp
// ============================================================
#include "Animation.h"

void Animation::update(float dt)
{
    if (m_frames.empty() || m_done) return;

    m_timer += dt;
    if (m_timer >= m_frameDuration)
    {
        m_timer -= m_frameDuration;
        m_current++;

        if (m_current >= static_cast<int>(m_frames.size()))
        {
            if (m_loop)
                m_current = 0;
            else
            {
                m_current = static_cast<int>(m_frames.size()) - 1;
                m_done    = true;
            }
        }
    }
}

void Animation::apply(sf::Sprite& sprite) const
{
    if (!m_frames.empty())
        sprite.setTextureRect(m_frames[m_current]);
}