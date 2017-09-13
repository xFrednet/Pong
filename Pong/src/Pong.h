#pragma once

#include "Renderer.h"

#define PONG_LEVEL_WIDTH     80
#define PONG_LEVEL_HEIGHT    48
#define PONG_PLAYER_SIZE     7

#define PONG_PLAYER_PADDING  3

#define PONG_PLAYER_1        0
#define PONG_PLAYER_2        1
#define PONG_PLAYER_1_X      (PONG_PLAYER_PADDING + 1)
#define PONG_PLAYER_2_X      (PONG_LEVEL_WIDTH - PONG_PLAYER_PADDING)
#define PONG_PLAYER_SPEED    0.33f
#define PONG_MAX_BALL_BOUNCE_SPEED_GAIN   0.3f

namespace pong {

	class Pong
	{
		float m_BallX;
		float m_BallY;
		float m_BallXM;
		float m_BallYM;

		float m_PlayerYPos[2];
		Renderer* m_Renderer;

	public:
		Pong(Renderer* renderer);
		
	private:
		void movePlayer(int player, int move);
		void moveBall();
		void draw();

	public:
		void update(int player1Move, int player2Move);
	};
}
