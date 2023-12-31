#pragma once

#include <atomic>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>

#include "Common.h"
#include "Core/KeyCode.h"
#include "Rendering/TextRenderer.h"

namespace Cypher
{
   using UpdateCallback = void (*)(float);
   using FixedUpdateCallback = void (*)(float);
   using RenderCallback = void (*)();

   template<typename T>
   concept ConsoleCallback = std::same_as<T, UpdateCallback> || std::same_as<T, RenderCallback> || std::same_as<T, FixedUpdateCallback>;

   class Console
   {
      struct Window;

   public:
      static constexpr uint32_t TARGET_UPDATES_PER_SECOND = 60;

      struct CYAPI InputState
      {
         bool isPressed = false;
         bool isReleased = false;
         bool isHeld = false;
      };

      static CYAPI bool Initialize(SHORT width, SHORT height, const std::wstring& title, SHORT fontWidth, SHORT fontHeight);
      static CYAPI void Start();

      static CYAPI void RegisterUpdateCallback(const UpdateCallback& callback) { GetInstance().m_updateCallback = callback; }
      static CYAPI void RegisterRenderCallback(const RenderCallback& callback) { GetInstance().m_renderCallback = callback; }
      static CYAPI void RegisterFixedUpdateCallback(const FixedUpdateCallback& callback) { GetInstance().m_fixedUpdateCallback = callback; }

      static inline CYAPI void Sleep(std::chrono::duration<float> duration)
      {
         GetInstance().m_sleeperAgent.shouldSleep = true;
         GetInstance().m_sleeperAgent.duration = duration;
      }

      template <typename Keycode_t>
      static inline CYAPI InputState GetKey(Keycode_t key) { return GetInstance().m_keyboard.currentKeyState[static_cast<size_t>(key)]; }

      template <typename Keycode_t>
      static inline CYAPI InputState GetMouseButton(Keycode_t button) { return GetInstance().m_mouse.currentButtonState[static_cast<size_t>(button)]; }
      static inline CYAPI short GetMouseX() { return GetInstance().m_mouse.x; }
      static inline CYAPI short GetMouseY() { return GetInstance().m_mouse.y; }

      static inline CYAPI const Window& GetWindow() { return GetInstance().m_window; }
      static inline CYAPI short GetWindowWidth() { return GetInstance().m_window.width; }
      static inline CYAPI short GetWindowHeight() { return GetInstance().m_window.height; }
      static inline CYAPI short GetWindowFocus() { return GetInstance().m_window.hasFocus; }
      static inline CYAPI const std::wstring& GetWindowTitle() { return GetInstance().m_window.title; }
   
      static inline CYAPI TextRenderer& GetRenderer() { return GetInstance().m_renderer; };

      static inline CYAPI bool IsUsingVSync() { return GetInstance().m_useVSync; }
      static inline CYAPI void UseVSync(bool vsync) { GetInstance().m_useVSync = vsync; }

   private:
      Console();
      ~Console();

      static CYAPI Console& GetInstance();

      void Loop();
      float SmoothDeltaTime(float deltaTime);
      void ProcessWindowEvents();
      void Update(float deltaTime);
      void FixedUpdate(float timeStep);
      void Render();
   
      static BOOL OnWindowsEvent(DWORD event);

      static std::atomic<bool> s_isRunning;
      static std::atomic<bool> s_terminateFlag;
      static std::mutex s_mutex;

      struct Window
      {
         SHORT width;
         SHORT height;
         std::wstring title;
         SMALL_RECT extents;
         bool hasFocus;

         Window() :
            width(0),
            height(0),
            extents({ 0, 0, 1, 1 }),
            hasFocus(true)
         {
         }

         Window(SHORT w, SHORT h, const std::wstring& t) :
            width(w),
            height(h),
            title(t),
            extents({ 0U, 0U, 1U, 1U }),
            hasFocus(true)
         {
         }

         inline SHORT Area() const { return width * height; }
      } m_window;

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

      UpdateCallback m_updateCallback;
      RenderCallback m_renderCallback;
      FixedUpdateCallback m_fixedUpdateCallback;

      struct SleeperAgent
      {
         bool shouldSleep = false;
         std::chrono::duration<float> duration = std::chrono::milliseconds(0);
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
