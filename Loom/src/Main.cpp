#include "Loom.h"

int main()
{
   Loom loom(L"CypherTest.dll");
   if (!loom.Initialize())
      return 1;
   
   loom.Run();

   return 0;
}