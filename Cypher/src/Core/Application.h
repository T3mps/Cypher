#pragma once

#include <string>
#include <Windows.h>

#include "Common.h"

namespace Cypher
{
   struct AppConfig
   {
      static constexpr SHORT DEFAULT_WINDOW_WIDTH = 96;
      static constexpr SHORT DEFAULT_WINDOW_HEIGHT = 64;
      static constexpr const wchar_t* DEFAULT_WINDOW_TITLE = L"Cypher";
      static constexpr SHORT DEFAULT_FONT_WIDTH = 8;
      static constexpr SHORT DEFAULT_FONT_HEIGHT = 12;

      SHORT windowWidth = DEFAULT_WINDOW_WIDTH;
      SHORT windowHeight = DEFAULT_WINDOW_HEIGHT;
      std::wstring windowTitle = DEFAULT_WINDOW_TITLE;
      SHORT fontWidth = DEFAULT_FONT_WIDTH;
      SHORT fontHeight = DEFAULT_FONT_HEIGHT;
   };

   static constexpr const char* FUNCTION_NAME_INITIALIZE = "Initialize";
   static constexpr const char* FUNCTION_NAME_UPDATE = "Update";
   static constexpr const char* FUNCTION_NAME_FIXED_UPDATE = "FixedUpdate";
   static constexpr const char* FUNCTION_NAME_RENDER = "Render";

   using InitializeFunction   =  bool(*)();
   using UpdateFunction       =  void(*)(float /* deltaTime/timestep */);
   using RenderFunction       =  void(*)();

   struct Application
   {
      AppConfig config;
      InitializeFunction initializeFunction;
      UpdateFunction updateFunction;
      UpdateFunction fixedUpdateFunction;
      RenderFunction renderFunction;
   };
}
