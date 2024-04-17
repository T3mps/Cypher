#pragma once

#include <map>
#include <functional>
#include <string>
#include <stdexcept>

#include "Module.h"

namespace Cypher
{
   class ModuleContext
   {
   public:
      using FunctionPtr = void(*)();
      
      ModuleContext() = default;
      ModuleContext(const std::wstring& name, const Module& mod);

      ~ModuleContext() = default;
      
      operator bool() const { return m_module.IsLoaded(); }

      template<typename Func>
      void RegisterFunction(const Module::identifier_t functionName)
      {
         if (m_functions.find(functionName) != m_functions.end())
         {
            // TODO: Log error
            return;
         }
         
         auto func = m_module.GetFunction<Func>(functionName);
         if (!func)
         {
            // TODO: Log error
            return;
         }

         m_functions[functionName] = reinterpret_cast<FunctionPtr>(func);
      }

      template<typename Func, typename... Args>
      bool InvokeFunction(Module::identifier_t functionName, Args... args)
      {
         auto it = m_functions.find(functionName);
         if (it == m_functions.end())
         {
            // TODO: Log error
            return false;
         }
         
         auto& rawFunction = it->second;
         if (!rawFunction)
         {
            // TODO: Log error
            return false;
         }

         Func castFunction = reinterpret_cast<Func>(rawFunction);
         castFunction(std::forward<Args>(args)...);
         return true;
      }

      const std::wstring& GetName() const { return m_moduleName; }
      const Module& GetModule() const { return m_module; }
      Module& GetModule() { return m_module; }
      const std::string& GetChecksum() const { return m_checksum; }

   private:
      std::wstring m_moduleName;
      Module m_module;
      std::string m_checksum;
      std::map<Module::identifier_t, FunctionPtr> m_functions;
   };
}
