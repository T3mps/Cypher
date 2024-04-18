#include <iostream>

#include "Common.h"
#include "Core/Logger.h"

CYAPI_EXTERN bool Initialize()
{
   LOG_INFO("Initialize", " called.");
   return true;
}

CYAPI_EXTERN void Update(float deltaTime)
{
   std::cout << "Update" << std::endl;
}

CYAPI_EXTERN void FixedUpdate(float timestep)
{
   std::cout << "FixedUpdate" << std::endl;
}

CYAPI_EXTERN void Render()
{
   std::cout << "Render" << std::endl;
}
