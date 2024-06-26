#include "Console.h"

#include <chrono>
#include <thread>

std::atomic<bool> Cypher::Console::s_isRunning(false);
std::atomic<bool> Cypher::Console::s_terminateFlag(false);
std::mutex Cypher::Console::s_mutex;

Cypher::Console::Console() :
   m_framesPerSecond(0U),
   m_updatesPerSecond(0U),
   m_fixedUpdatesPerSecond(0U),
   m_useVSync(false),
   m_screenBuffer(nullptr),
   m_renderer(m_window.width, m_window.height, m_screenBuffer),
   m_restoreHandle(nullptr),
   m_inputHandle(GetStdHandle(STD_INPUT_HANDLE)),
   m_outputHandle(GetStdHandle(STD_OUTPUT_HANDLE)),
   m_initialized(false),
   m_initializeFunction(nullptr),
   m_updateFunction(nullptr),
   m_fixedUpdateFunction(nullptr),
   m_renderFunction(nullptr)
{
}

Cypher::Console::~Console()
{
   SetConsoleActiveScreenBuffer(m_restoreHandle);
   delete[] m_screenBuffer;
}

Cypher::Console& Cypher::Console::GetInstance()
{
   static Console console;
   return console;
}

bool Cypher::Console::Initialize(Application application)
{
   Console& instance = GetInstance();
   std::call_once(instance.m_initFlag, [&]()
   {
      if (instance.m_outputHandle == INVALID_HANDLE_VALUE || instance.m_inputHandle == INVALID_HANDLE_VALUE)
         return false;
      
      const AppConfig& config = application.config;

      instance.m_window = { config.windowWidth, config.windowHeight, config.windowTitle };
      SetConsoleWindowInfo(instance.m_outputHandle, TRUE, &instance.m_window.extents);

      COORD coord = { instance.m_window.width, instance.m_window.height };
      if (!SetConsoleScreenBufferSize(instance.m_outputHandle, coord))
         return false;

      if (!SetConsoleActiveScreenBuffer(instance.m_outputHandle))
         return false;

      CONSOLE_FONT_INFOEX fontInfo;
      fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
      fontInfo.nFont = 0;
      fontInfo.dwFontSize.X = config.fontWidth;
      fontInfo.dwFontSize.Y = config.fontHeight;
      fontInfo.FontFamily = FF_DONTCARE;
      fontInfo.FontWeight = FW_NORMAL;

      wcscpy_s(fontInfo.FaceName, L"Consolas");
      if (!SetCurrentConsoleFontEx(instance.m_outputHandle, false, &fontInfo))
         return false;

      CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
      if (!GetConsoleScreenBufferInfo(instance.m_outputHandle, &screenBufferInfo))
         return false;
      if (instance.m_window.width > screenBufferInfo.dwMaximumWindowSize.X || instance.m_window.height > screenBufferInfo.dwMaximumWindowSize.Y)
         return false;

      instance.m_window.extents = { 0, 0, static_cast<SHORT>(instance.m_window.width - 1), static_cast<SHORT>(instance.m_window.height - 1) };
      if (!SetConsoleWindowInfo(instance.m_outputHandle, TRUE, &instance.m_window.extents))
         return false;

      if (!SetConsoleMode(instance.m_inputHandle, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
         return false;

      instance.m_screenBuffer = new CHAR_INFO[instance.m_window.Area()];
      if (!instance.m_screenBuffer)
         return false;
      memset(instance.m_screenBuffer, 0, sizeof(CHAR_INFO) * instance.m_window.Area());

      SetConsoleCtrlHandler(static_cast<PHANDLER_ROUTINE>(OnWindowsEvent), TRUE);

      instance.m_initializeFunction = application.initializeFunction;
      instance.m_updateFunction = application.updateFunction;
      instance.m_fixedUpdateFunction = application.fixedUpdateFunction;
      instance.m_renderFunction = application.renderFunction;

      instance.m_initialized = instance.m_initializeFunction();
      return true;
   });
   
   return instance.m_initialized;
}

void Cypher::Console::Start()
{
   s_isRunning = true;
   std::thread(&Cypher::Console::Loop, &GetInstance()).join();
}

void Cypher::Console::Update(float deltaTime)
{
   if (m_updateFunction)
      m_updateFunction(deltaTime);
}

void Cypher::Console::FixedUpdate(float timeStep)
{
   if (m_fixedUpdateFunction)
      m_fixedUpdateFunction(timeStep);
}

void Cypher::Console::Render()
{
   wchar_t s[256];
   swprintf_s(s, 256, L"%s | fps: %d, ups: %d, fups: %d", m_window.title.c_str(), m_framesPerSecond, m_updatesPerSecond, m_fixedUpdatesPerSecond);
   SetConsoleTitle(s);
   m_renderer.Clear();
   if (m_renderFunction)
      m_renderFunction();
   WriteConsoleOutput(m_outputHandle, m_screenBuffer, { m_window.width, m_window.height }, { 0, 0 }, &m_window.extents);
}

BOOL Cypher::Console::OnWindowsEvent(DWORD event)
{
   switch (event)
   {
      case CTRL_CLOSE_EVENT:
      {
         s_isRunning.store(false, std::memory_order_release);
         while (!s_terminateFlag.load(std::memory_order_acquire))
         {
            std::this_thread::yield();
         }
         break;
      }
      default:
         break;
   }
   return TRUE;
}

void Cypher::Console::Loop()
{
   using std::chrono::steady_clock;
   using std::chrono::time_point;
   using std::chrono::duration;
   using std::chrono::milliseconds;
   using std::chrono::duration_cast;

   constexpr duration<float> fixedUpdateRate = milliseconds(1000 / TARGET_UPDATES_PER_SECOND);
   constexpr float maxAccumulatedTime = fixedUpdateRate.count() * 5; // max 5 updates accumulated
   time_point<steady_clock> lastUpdateTime = steady_clock::now();
   time_point<steady_clock> lastFixedUpdateTime = steady_clock::now();
   time_point<steady_clock> lastSecond = steady_clock::now();

   uint32_t frames = 0;
   uint32_t updates = 0;
   uint32_t fixedUpdates = 0;
   duration<float> accumulator = duration<float>::zero();

   while (s_isRunning)
   {
      ProcessWindowEvents();

      auto now = steady_clock::now();
      auto frameElapsedTime = duration_cast<duration<float>>(now - lastUpdateTime);
      lastUpdateTime = now;

      float smoothedDeltaTime = SmoothDeltaTime(frameElapsedTime.count());
      Update(smoothedDeltaTime);
      ++updates;

      auto fixedUpdateElapsedTime = duration_cast<duration<float>>(now - lastFixedUpdateTime);
      lastFixedUpdateTime = now;
      accumulator += fixedUpdateElapsedTime;
      
      if (accumulator.count() > maxAccumulatedTime)
         accumulator = duration<float>(maxAccumulatedTime);

      while (accumulator >= fixedUpdateRate)
      {
         FixedUpdate(fixedUpdateRate.count());
         accumulator -= fixedUpdateRate;
         ++fixedUpdates;
      }

      Render();
      ++frames;

      if (duration_cast<duration<float>>(now - lastSecond).count() >= 1.0f)
      {
         m_framesPerSecond = frames;
         m_updatesPerSecond = updates;
         m_fixedUpdatesPerSecond = fixedUpdates;

         frames = 0;
         updates = 0;
         fixedUpdates = 0;
         lastSecond = now;
      }

      if (m_sleeperAgent.shouldSleep)
      {
         std::this_thread::sleep_for(m_sleeperAgent.duration);
         m_sleeperAgent.shouldSleep = false;
      }

      if (m_useVSync)
         std::this_thread::sleep_for(milliseconds(1));
   }

   SetConsoleActiveScreenBuffer(m_restoreHandle);
}

float Cypher::Console::SmoothDeltaTime(float deltaTime)
{
   constexpr float targetFrameLength = 1 / TARGET_UPDATES_PER_SECOND;
   static float smoothedDeltaTime = targetFrameLength;
   smoothedDeltaTime = 0.9f * smoothedDeltaTime + 0.1f * deltaTime;
   return smoothedDeltaTime;
}

void Cypher::Console::ProcessWindowEvents()
{
   for (size_t i = 0; i < Keyboard::KEY_COUNT; ++i)
   {
      m_keyboard.newKeyState[i] = GetAsyncKeyState(static_cast<int>(i)) & 0x8000;
      m_keyboard.currentKeyState[i].isPressed = (m_keyboard.newKeyState[i] && !m_keyboard.oldKeyState[i]);
      m_keyboard.currentKeyState[i].isReleased = (!m_keyboard.newKeyState[i] && m_keyboard.oldKeyState[i]);
      m_keyboard.currentKeyState[i].isHeld = m_keyboard.newKeyState[i];
      m_keyboard.oldKeyState[i] = m_keyboard.newKeyState[i];
   }

   INPUT_RECORD eventBuffer[32];
   DWORD eventCount = 0;
   
   GetNumberOfConsoleInputEvents(m_inputHandle, &eventCount);

   if (eventCount > 0)
   {
      ReadConsoleInput(m_inputHandle, eventBuffer, eventCount, &eventCount);
      for (DWORD i = 0; i < eventCount; ++i)
      {
         switch (eventBuffer[i].EventType)
         {
            case FOCUS_EVENT:
               m_window.hasFocus = eventBuffer[i].Event.FocusEvent.bSetFocus;
               break;
            case MOUSE_EVENT:
               MOUSE_EVENT_RECORD mouseEvent = eventBuffer[i].Event.MouseEvent;
               switch (mouseEvent.dwEventFlags)
               {
                  case MOUSE_MOVED:
                     m_mouse.x = mouseEvent.dwMousePosition.X;
                     m_mouse.y = mouseEvent.dwMousePosition.Y;
                     break;
                  case 0:
                     for (size_t j = 0; j < Mouse::BUTTON_COUNT; ++j)
                     {
                        m_mouse.newButtonState[j] = (mouseEvent.dwButtonState & (1 << j)) != 0;
                     }
                     break;
                  default:
                     break;
               }
               break;
            default:
               break;
         }
      }
   }

   for (size_t i = 0; i < Mouse::BUTTON_COUNT; ++i)
   {
      m_mouse.currentButtonState[i].isPressed = m_mouse.newButtonState[i] && !m_mouse.oldButtonState[i];
      m_mouse.currentButtonState[i].isReleased = !m_mouse.newButtonState[i] && m_mouse.oldButtonState[i];
      m_mouse.currentButtonState[i].isHeld = m_mouse.newButtonState[i];
      m_mouse.oldButtonState[i] = m_mouse.newButtonState[i];
   }
}
