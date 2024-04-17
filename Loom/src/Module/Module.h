#pragma once

#include <string>
#include <Windows.h>
#include <vector>

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
            // TODO: Log error
            return nullptr;
         }

         void* functionPtr = GetProcAddress(m_hModule, functionName);
         if (!functionPtr)
         {
            // TODO: Log error
            return nullptr;
         }

         return reinterpret_cast<Func>(functionPtr);
      }

      bool HasFunction(identifier_t functionName) const
      {
         if (!m_hModule)
         {
            // TODO: Log error
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
