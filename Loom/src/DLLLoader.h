#include <windows.h>
#include <string>
#include <iostream>

class DLLLoader
{
public:
   DLLLoader(const std::wstring& libraryPath) :
      m_libraryPath(libraryPath),
      m_hModule(nullptr)
   {}

   ~DLLLoader()
   {
      if (m_hModule)
      {
         FreeLibrary(m_hModule);
         m_hModule = nullptr;
      }
   }

   bool Load()
   {
      m_hModule = LoadLibraryW(m_libraryPath.c_str());
      if (!m_hModule)
      {
         std::wcerr << L"Failed to load library: " << m_libraryPath << std::endl;
         return false;
      }
      return true;
   }

   template<typename Func>
   Func GetFunction(const char* functionName)
   {
      void* funcPtr = GetProcAddress(m_hModule, functionName);
      if (!funcPtr)
      {
         std::cerr << "Failed to get function: " << functionName << std::endl;
         return nullptr;
      }
      return reinterpret_cast<Func>(funcPtr);
   }

private:
   std::wstring m_libraryPath;
   HMODULE m_hModule;
};
