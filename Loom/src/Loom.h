#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Module/Module.h"
#include "Module/ModuleContext.h"
#include "Types.h"

namespace Cypher
{
   class Loom
   {
   public:
      static constexpr Module::identifier_t FUNCTION_NAME_INITIALIZE    =  "Initialize";
      static constexpr Module::identifier_t FUNCTION_NAME_UPDATE        =  "Update";
      static constexpr Module::identifier_t FUNCTION_NAME_FIXED_UPDATE  =  "FixedUpdate";
      static constexpr Module::identifier_t FUNCTION_NAME_RENDER        =  "Render";

      using FunctionInitialize   =  void (*)();
      using FunctionUpdate       =  void (*)(float /* deltaTime/timestep */ );
      using FunctionRender       =  void (*)();
      
      using ModuleList = std::vector<ModuleContext>;

      Loom(const std::wstring& moduleName);
      ~Loom();

      void Initialize();
      
      void Start();

   private:
      void LoadMainModule(const std::wstring& moduleName);
      
      ModuleContext* LoadModule(const std::wstring& moduleName);
      void UnloadModule(ModuleContext& moduleData);

      ModuleContext* m_mainModule;
      ModuleList m_modules;
   };
}
