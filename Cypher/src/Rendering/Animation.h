#pragma once

#include <chrono>
#include <vector>

#include "Sprite.h"

namespace Cypher
{
   class Animation
   {
   public:
      struct Frame
      {
         Sprite sprite;
         float duration;

         Frame(const Sprite& s, float d = 0.1f) : sprite(s), duration(d) {}

         bool operator==(const Frame& other) const;
      };

      using Clock = std::chrono::high_resolution_clock;
      using TimePoint = std::chrono::time_point<Clock>;

      Animation();
      ~Animation() = default;

      const Frame& operator[](size_t index) const;

      bool operator==(const Animation& other) const;
      bool operator!=(const Animation& other) const;

      void AddFrame(const Frame& frame);
      void RemoveFrame(size_t index);

      void Play(bool loop = false);
      void Pause();
      void Stop();

      void Update();

      void UseUniformFrameDuration(float duration);
      void UseIndividualFrameDurations();

      const Frame& GetCurrentFrame() const { return m_frames[m_currentFrameIndex]; }
      const size_t& GetFrameCount() const { return m_frames.size(); }

   private:
      std::vector<Frame> m_frames;
      size_t m_currentFrameIndex;
      bool m_isPlaying;
      bool m_isLooping;
      TimePoint m_lastFrameChangeTime;
      bool m_useUniformDuration;
      float m_uniformDuration;
   };
} // namespace Cypher
