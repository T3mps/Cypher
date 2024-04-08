#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "Console.h"
#include "Core/Application.h"
#include "DLLLoader.h"
#include "Utils.h"

class TelemetryOutputter
{
public:
   TelemetryOutputter() {}
   ~TelemetryOutputter() = default;

   void Initialize(const std::string& versionString)
   {
      std::string header = "\n==================\n LOOM " + versionString + "\n==================\n";
      std::cout << header << std::endl;
      Output("Initializing...");
   }

   template <typename... Args>
   void Output(const Args&... args)
   {
      std::ostringstream oss;
      Concatenate(oss, args...);
      std::cout << " -" << oss.str() << std::endl;
   }

   template <typename... Args>
   void Error(const Args&... args)
   {
      std::ostringstream oss;
      Concatenate(oss, args...);
      std::cerr << " -" << oss.str() << std::endl;
   }

private:
   template <typename T>
   void Concatenate(std::ostringstream& oss, const T& value) { oss << value; }

   template <typename... Args>
   void Concatenate(std::ostringstream& oss, const Args&... args) { (oss << ... << args); }
};

class Loom
{
public:
   static constexpr const char* TARGET_FUNCTION_NAME = "CreateGameInstance";

   Loom(const std::wstring& gameModulePath) :
      m_appInstance(nullptr),
      m_version(),
      m_gameModulePath(gameModulePath),
      m_initialized(false)
   {}

   bool Initialize()
   {
      if (!m_initialized)
      {
         m_telemetryOut.Initialize(m_version.GetString());

         if (LoadGameModule())
         {
            m_initialized = true;
            m_telemetryOut.Output("Loom successfully initialized");
            return true;
         }
      }
      else
      {
         m_telemetryOut.Error("Loom has already been initialized");
      }
      
      return false;
   }

   void Run()
   {
      if (m_initialized)
      {
         Cypher::Console::Start();
      }
      else
      {
         m_telemetryOut.Error("Loom not initialized");
      }
   }

private:
   bool LoadGameModule()
   {
      DLLLoader loader(m_gameModulePath);
      m_telemetryOut.Output("DLL Loader initialized");

      if (!loader.Load())
         return false;
      m_telemetryOut.Output(Utils::WStringToString(m_gameModulePath), " successfully loaded");

      using CreateGameInstanceFunc = Cypher::Application* (*)();
      CreateGameInstanceFunc createGameInstance = loader.GetFunction<CreateGameInstanceFunc>(TARGET_FUNCTION_NAME);
      if (!createGameInstance)
         return false;
      m_telemetryOut.Output("Entry point function ", TARGET_FUNCTION_NAME, "@0x", createGameInstance, " found");
      
      if (Cypher::Application* appInstance = createGameInstance())
      {
         m_appInstance.reset(appInstance);
         m_telemetryOut.Output("Application instance 0x", appInstance, " created");

         if (!Cypher::Console::Initialize(m_appInstance))
         {
            m_telemetryOut.Error("Failed to create the game instance!");
            return false;
         }

         m_telemetryOut.Output("Instance 0x", appInstance, " bound to Cypher");
         m_telemetryOut.Output("Cypher successfully initialized");
         return true;
      }

      return false;
   }

   struct Version
   {
      const long major     = 1;
      const long minor     = 0;
      const long revision  = 0;

      const std::string& GetString()
      {
         static std::string version = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(revision);
         return version;
      }
   } m_version;

   std::shared_ptr<Cypher::Application> m_appInstance;
   std::wstring m_gameModulePath;
   bool m_initialized;
   TelemetryOutputter m_telemetryOut;
};
