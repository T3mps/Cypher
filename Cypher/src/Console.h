#pragma once

#include <atomic>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "Common.h"
#include "Core/Application.h"
#include "Core/KeyCode.h"
#include "Core/Window.h"
#include "Rendering/TextRenderer.h"
#include "Types.h"

namespace Cypher
{
   class Console
   {
   public:
      static constexpr uint32_t TARGET_UPDATES_PER_SECOND = 60;

      struct InputState
      {
         bool isPressed = false;
         bool isReleased = false;
         bool isHeld = false;
      };

      static bool Initialize(Application application);
      static void Start();
      
      static inline void Sleep(std::chrono::duration<float32_t> duration)
      {
         GetInstance().m_sleeperAgent.shouldSleep = true;
         GetInstance().m_sleeperAgent.duration = duration;
      }

      template <typename Keycode_t>
      static inline InputState GetKey(Keycode_t key) { return GetInstance().m_keyboard.currentKeyState[static_cast<size_t>(key)]; }

      template <typename Keycode_t>
      static inline InputState GetMouseButton(Keycode_t button) { return GetInstance().m_mouse.currentButtonState[static_cast<size_t>(button)]; }
      static inline short GetMouseX() { return GetInstance().m_mouse.x; }
      static inline short GetMouseY() { return GetInstance().m_mouse.y; }

      static inline const Window& GetWindow() { return GetInstance().m_window; }
      static inline short GetWindowWidth() { return GetInstance().m_window.width; }
      static inline short GetWindowHeight() { return GetInstance().m_window.height; }
      static inline short GetWindowFocus() { return GetInstance().m_window.hasFocus; }
      static inline const std::wstring& GetWindowTitle() { return GetInstance().m_window.title; }
   
      static inline TextRenderer& GetRenderer() { return GetInstance().m_renderer; };

      static inline bool IsUsingVSync() { return GetInstance().m_useVSync; }
      static inline void UseVSync(bool vsync) { GetInstance().m_useVSync = vsync; }

      static inline bool IsInitialized() { return GetInstance().m_initialized; }

   private:
      Console();
      ~Console();

      static Console& GetInstance();

      void Loop();
      float32_t SmoothDeltaTime(float32_t deltaTime);
      void ProcessWindowEvents();
      void Update(float32_t deltaTime);
      void FixedUpdate(float32_t timeStep);
      void Render();
   
      static BOOL OnWindowsEvent(DWORD event);

      static std::atomic<bool> s_isRunning;
      static std::atomic<bool> s_terminateFlag;
      static std::mutex s_mutex;

      Window m_window;
      
      InitializeFunction m_initializeFunction;
      UpdateFunction m_updateFunction;
      UpdateFunction m_fixedUpdateFunction;
      RenderFunction m_renderFunction;

      uint32_t m_framesPerSecond;
      uint32_t m_updatesPerSecond;
      uint32_t m_fixedUpdatesPerSecond;
      bool m_useVSync;

      TextRenderer::Buffer m_screenBuffer;
      TextRenderer m_renderer;
      HANDLE m_restoreHandle;
      HANDLE m_inputHandle;
      HANDLE m_outputHandle;

      std::once_flag m_initFlag;
      
      bool m_initialized;

      struct SleeperAgent
      {
         bool shouldSleep = false;
         std::chrono::duration<float32_t> duration = std::chrono::milliseconds(0);
      } m_sleeperAgent;

      struct Keyboard
      {
         static const size_t KEY_COUNT = 256;

         InputState currentKeyState[KEY_COUNT] = { InputState() };
         short oldKeyState[KEY_COUNT] = { 0 };
         short newKeyState[KEY_COUNT] = { 0 };

      } m_keyboard;
      struct Mouse
      {
         static const size_t BUTTON_COUNT = 5;

         SHORT x = 0;
         SHORT y = 0;
    
         InputState currentButtonState[BUTTON_COUNT] = { InputState() };
         bool oldButtonState[BUTTON_COUNT] = { false };
         bool newButtonState[BUTTON_COUNT] = { false };
      } m_mouse;
   };
} // namespace Cypher
