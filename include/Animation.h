// ============================================================
// Animation.h — Simple sprite-sheet animation
//
// Stores a list of texture sub-rects (frames).
// Call update() every frame, it advances to the next frame
// when the timer exceeds the frame duration.
// Call apply() to set the current frame on a sprite.
//
// Usage:
//   Animation anim;
//   anim.addFrame({0,   0, 32, 48});
//   anim.addFrame({32,  0, 32, 48});
//   anim.addFrame({64,  0, 32, 48});
//   anim.setFrameDuration(0.12f);
//   anim.setLoop(true);
//   ...
//   anim.update(dt);
//   anim.apply(sprite);
// ============================================================
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Animation
{
public:
    Animation() = default;

    void addFrame(sf::IntRect rect)  { m_frames.push_back(rect); }
    void setFrameDuration(float sec) { m_frameDuration = sec; }
    void setLoop(bool loop)          { m_loop = loop; }
    void reset()                     { m_current = 0; m_timer = 0.f; m_done = false; }

    void update(float dt);
    void apply(sf::Sprite& sprite) const;

    bool isDone() const { return m_done; }
    bool hasFrames() const { return !m_frames.empty(); }

private:
    std::vector<sf::IntRect> m_frames;
    float m_frameDuration{0.12f};   // seconds per frame
    float m_timer{0.f};
    int   m_current{0};
    bool  m_loop{true};
    bool  m_done{false};
};