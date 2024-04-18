#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Core/Application.h"
#include "Module/Module.h"
#include "Module/ModuleContext.h"
#include "Types.h"

namespace Cypher
{
   class Loom
   {
   public:
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
