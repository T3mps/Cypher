#include "Animation.h"

#include <iterator>

bool Cypher::Animation::Frame::operator==(const Frame& other) const
{
   return sprite == other.sprite && duration == other.duration;
}

Cypher::Animation::Animation() :
   m_currentFrameIndex(0),
   m_isPlaying(false),
   m_isLooping(false),
   m_lastFrameChangeTime(),
   m_useUniformDuration(false),
   m_uniformDuration(0.0f)
{
}

const Cypher::Animation::Frame& Cypher::Animation::operator[](size_t index) const
{
   if (index >= m_frames.size())
      throw std::out_of_range("Index out of range");
   return m_frames[index];
}

bool Cypher::Animation::operator==(const Animation& other) const
{
   return m_frames == other.m_frames &&
          m_currentFrameIndex == other.m_currentFrameIndex &&
          m_isPlaying == other.m_isPlaying &&
          m_isLooping == other.m_isLooping;
}

bool Cypher::Animation::operator!=(const Animation& other) const
{
   return !(*this == other);
}

void Cypher::Animation::AddFrame(const Frame& frame)
{
   m_frames.push_back(frame);
}

void Cypher::Animation::RemoveFrame(size_t index)
{
   if (index < m_frames.size())
   {
      auto frameIt = m_frames.begin();
      std::advance(frameIt, index);
      m_frames.erase(frameIt);
   }
}

void Cypher::Animation::Play(bool loop)
{
   m_isPlaying = true;
   m_isLooping = loop;
   m_lastFrameChangeTime = Clock::now();
}

void Cypher::Animation::Pause()
{
   m_isPlaying = false;
}

void Cypher::Animation::Stop()
{
   m_isPlaying = false;
   m_isLooping = false;
   m_currentFrameIndex = 0;
}

void Cypher::Animation::Update()
{
   if (!m_isPlaying || m_frames.empty())
      return;

   auto now = Clock::now();
   float frameDuration = m_useUniformDuration ? m_uniformDuration : m_frames[m_currentFrameIndex].duration;
   float elapsedSecs = std::chrono::duration<float>(now - m_lastFrameChangeTime).count();

   if (elapsedSecs > m_frames[m_currentFrameIndex].duration)
   {
      m_currentFrameIndex = (m_currentFrameIndex + 1) % m_frames.size();
      m_lastFrameChangeTime = now;

      if (m_currentFrameIndex == 0 && !m_isLooping)
      {
         Stop();
         m_currentFrameIndex = m_frames.size() - 1;
      }
   }
}

void Cypher::Animation::UseUniformFrameDuration(float duration)
{
   m_useUniformDuration = true;
   m_uniformDuration = duration;
}

void Cypher::Animation::UseIndividualFrameDurations()
{
   m_useUniformDuration = false;
}