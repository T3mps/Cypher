#include "ModuleContext.h"

#include "ModuleLoader.h"

Cypher::ModuleContext::ModuleContext(const std::wstring& name, const Module& mod) :
   m_moduleName(name),
   m_module(mod),
   m_checksum(ModuleLoader::ComputeChecksum(name))
{}
