#pragma once

#include <Windows.h>

#include "Types.h"

namespace Cypher
{
   class Application
   {
   public:
      struct Config
      {
         static constexpr SHORT DEFAULT_WINDOW_WIDTH = 96;
         static constexpr SHORT DEFAULT_WINDOW_HEIGHT = 64;
         static constexpr SHORT DEFAULT_FONT_WIDTH = 8;
         static constexpr SHORT DEFAULT_FONT_HEIGHT = 12;
         
         SHORT windowWidth = DEFAULT_WINDOW_WIDTH;
         SHORT windowHeight = DEFAULT_WINDOW_HEIGHT;
         std::wstring windowTitle = L"Cypher";
         SHORT fontWidth = DEFAULT_FONT_WIDTH;
         SHORT fontHeight = DEFAULT_FONT_HEIGHT;
      };

      Application() : m_config() {}
      virtual ~Application() = default;

      virtual void Initialize() = 0;
      virtual void Update(float32_t deltaTime) = 0;
      virtual void FixedUpdate(float32_t timeStep) = 0;
      virtual void Render() = 0;
      
   protected:
      Config m_config;
   };
}