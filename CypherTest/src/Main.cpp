#include "Breakout.h"

int main()
{
	Cypher::Console::Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, L"Breakout", 8, 8);

	Cypher::Console::RegisterUpdateCallback(&Update);
	Cypher::Console::RegisterFixedUpdateCallback(&FixedUpdate);
	Cypher::Console::RegisterRenderCallback(&Render);

	Cypher::Console::Start();

	return 0;
}
