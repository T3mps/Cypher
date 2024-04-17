#include "Loom.h"

#include "Module/ModuleLoader.h"
#include "Module/ModuleContext.h"

Cypher::Loom::Loom(const std::wstring& mainModuleName)
{
   LoadMainModule(mainModuleName);
}

Cypher::Loom::~Loom()
{
   for (auto& mod : m_modules)
   {
      UnloadModule(mod);
   }
}

void Cypher::Loom::Initialize()
{
   if (!m_mainModule)
   {
      // TODO: Log error
      return;
   }

   m_mainModule->InvokeFunction<FunctionInitialize>(FUNCTION_NAME_INITIALIZE);
}

void Cypher::Loom::Start()
{
   for (int i = 0; i < 10; i++)
   {
      m_mainModule->InvokeFunction<FunctionUpdate>(FUNCTION_NAME_UPDATE, i);
      m_mainModule->InvokeFunction<FunctionUpdate>(FUNCTION_NAME_FIXED_UPDATE, i);
      m_mainModule->InvokeFunction<FunctionRender>(FUNCTION_NAME_RENDER);
   }
}

void Cypher::Loom::LoadMainModule(const std::wstring& moduleName)
{
   ModuleContext* module = LoadModule(moduleName);
   if (!module)
   {
      // TODO: Log error
      return;
   }
   
   m_mainModule = module;

   m_mainModule->RegisterFunction<FunctionInitialize>(FUNCTION_NAME_INITIALIZE);
   m_mainModule->RegisterFunction<FunctionUpdate>(FUNCTION_NAME_UPDATE);
   m_mainModule->RegisterFunction<FunctionUpdate>(FUNCTION_NAME_FIXED_UPDATE);
   m_mainModule->RegisterFunction<FunctionRender>(FUNCTION_NAME_RENDER);
}

Cypher::ModuleContext* Cypher::Loom::LoadModule(const std::wstring& moduleName)
{
   Module mod = ModuleLoader::Load(moduleName);
   if (!mod.IsLoaded())
   {
      // TODO: Log error
      return nullptr;
   }

   ModuleContext moduleCtx(moduleName, mod);
   m_modules.push_back(moduleCtx);
   return &m_modules.back();
}

void Cypher::Loom::UnloadModule(ModuleContext& moduleCtx)
{
   ModuleLoader::Unload(moduleCtx.GetModule());
}
