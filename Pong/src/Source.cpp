#include <iostream>

#include <GameState.h>
#include <PongRenderer.h>
#include <CrappyAI.h>

#include <thread>

using namespace std;
using namespace pong;
using namespace cria;

int pixels_[PONG_LEVEL_HEIGHT * PONG_LEVEL_WIDTH];

void getPlayerMoves(int* p1Move, int* p2Move)
{
	*p1Move = 0;
	*p2Move = 0;

	//player 1
	if (GetAsyncKeyState('W'))
		(*p1Move)--;
	if (GetAsyncKeyState('S'))
		(*p1Move)++;

	//player 2
	if (GetAsyncKeyState(VK_UP))
		(*p2Move)--;
	if (GetAsyncKeyState(VK_DOWN))
		(*p2Move)++;
}
void start()
{
	Renderer r(PONG_LEVEL_WIDTH, PONG_LEVEL_HEIGHT, "Pong");
	
	NeuronNetwork nNetwork;
	GameState pong;
	int player1Move;
	int player2Move;
	int u = 0;
	while (r.updateWindow())
	{
		getPlayerMoves(&player1Move, &player2Move);

		pong.update(player1Move, player2Move);
		pong.draw(pixels_);

		if (true)
		{
			nNetwork.processBitmap(pixels_, PONG_LEVEL_WIDTH, PONG_LEVEL_HEIGHT);
		} else
		{
			r.loadPixels((char*)pixels_);
			r.drawTexture();
			r.swapBuffers();
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
	}
}

int main()
{
	cout << "Hello" << endl;

	cria::test_xml_save();
	
	//start();

	cin.get();
	return 0;
}