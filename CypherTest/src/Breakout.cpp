#include "Breakout.h"

extern "C" CYAPI Cypher::Application* CreateGameInstance()
{
	return new Game();
}
