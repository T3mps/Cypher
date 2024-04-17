#include "ModuleLoader.h"

#include <fstream>
#include <iostream>

#include "Util/Hash.h"
#include "Util/StringUtil.h"

Cypher::Module Cypher::ModuleLoader::Load(const std::wstring& moduleName)
{
   Module module;
   HMODULE hModule = LoadLibrary(moduleName.c_str());
   if (!hModule)
   {
      // TODO: Log error
      return module;
   }

   module.SetHandle(hModule);
   return module;
}

void Cypher::ModuleLoader::Unload(Module& module)
{
   if (module.IsLoaded())
   {
      FreeLibrary(module.GetHandle());
      module.SetHandle(nullptr);
   }
}

std::string Cypher::ModuleLoader::ComputeChecksum(const std::wstring& moduleName)
{
   std::vector<unsigned char> fileData = ReadFileBinary(moduleName);
   if (fileData.empty())
   {
      //std::cerr << "Failed to read file data." << std::endl;
      return "";
   }

   std::vector<unsigned char> hash(picosha2::k_digest_size);
   Cypher::Hash256(fileData.begin(), fileData.end(), hash.begin(), hash.end());

   return Cypher::BytesToHexString(hash.begin(), hash.end());
}

std::vector<unsigned char> Cypher::ModuleLoader::ReadFileBinary(const std::wstring& moduleName)
{
   std::ifstream file(moduleName, std::ios::binary | std::ios::ate);
   if (!file)
      return {};

   std::streamsize size = file.tellg();
   file.seekg(0, std::ios::beg);

   std::vector<unsigned char> buffer(size);
   if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
      return {};

   return buffer;
}
