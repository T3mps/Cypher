#include "Loom.h"

#include "Console.h"
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

   Application app;
   app.initializeFunction = m_mainModule->GetFunction<InitializeFunction>(FUNCTION_NAME_INITIALIZE);
   app.updateFunction = m_mainModule->GetFunction<UpdateFunction>(FUNCTION_NAME_UPDATE);
   app.fixedUpdateFunction = m_mainModule->GetFunction<UpdateFunction>(FUNCTION_NAME_FIXED_UPDATE);
   app.renderFunction = m_mainModule->GetFunction<RenderFunction>(FUNCTION_NAME_RENDER);
   
   Console::Initialize(app);
}

void Cypher::Loom::Start()
{
   Console::Start();
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

   m_mainModule->RegisterFunction<InitializeFunction>(FUNCTION_NAME_INITIALIZE);
   m_mainModule->RegisterFunction<UpdateFunction>(FUNCTION_NAME_UPDATE);
   m_mainModule->RegisterFunction<UpdateFunction>(FUNCTION_NAME_FIXED_UPDATE);
   m_mainModule->RegisterFunction<RenderFunction>(FUNCTION_NAME_RENDER);
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
