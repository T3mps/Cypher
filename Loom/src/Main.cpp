#include "Loom.h"

int main()
{
   Cypher::Loom loom(L"CypherTest.dll");
   loom.Initialize();
   loom.Start();
   return 0;
}