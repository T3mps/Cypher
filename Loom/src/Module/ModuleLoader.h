#pragma once

#include <string>
#include <vector>

#include "Module.h"
#include "Types.h"

namespace Cypher
{
   class ModuleLoader
   {
   public:
      static Module Load(const std::wstring& moduleName);
      static void Unload(Module& module);
      
      static std::string ComputeChecksum(const std::wstring& moduleName);
      
   private:
      static std::vector<unsigned char> ReadFileBinary(const std::wstring& filePath);

      ModuleLoader() = default;
      ~ModuleLoader() = default;

      ModuleLoader(const ModuleLoader&) = delete;
      ModuleLoader& operator=(const ModuleLoader&) = delete;
   };
}
