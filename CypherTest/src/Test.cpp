#include <iostream>

#include "Common.h"

CYAPI_EXTERN void Initialize()
{
   std::cout << "Initialize" << std::endl;
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
