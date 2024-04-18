#pragma once

#include <string>
#include <Windows.h>
#include <vector>

#include "Core/Logger.h"
#include "Types.h"

namespace Cypher
{
   class Module
   {
   public:
      using identifier_t = const char*;

      Module();
      virtual ~Module() = default;

      template<typename Func>
      Func GetFunction(identifier_t functionName) const
      {
         if (!m_hModule)
         {
            LOG_ERROR("Attempting to get function \"", functionName, "\" from a null module. Try loading the module first.");
            return nullptr;
         }

         void* functionPtr = GetProcAddress(m_hModule, functionName);
         if (!functionPtr)
         {
            LOG_ERROR("Failed to get function \"", functionName, "\". Error code: ", GetLastError());
            return nullptr;
         }

         LOG_INFO("Successfully retrieved function \"", functionName, "\".");
         return reinterpret_cast<Func>(functionPtr);
      }

      bool HasFunction(identifier_t functionName) const
      {
         if (!m_hModule)
         {
            LOG_ERROR("Attempting to get function \"", functionName, "\" from a null module. Try loading the module first.");
            return false;
         }

         void* functionPtr = GetProcAddress(m_hModule, functionName);
         return functionPtr != nullptr;
      }

      bool IsLoaded() const { return m_hModule != nullptr; }
      
      HMODULE GetHandle() const { return m_hModule; }
      void SetHandle(HMODULE hModule) { m_hModule = hModule; }
      
   private:
      HMODULE m_hModule;
   };
}
