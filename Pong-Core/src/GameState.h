#pragma once

#define PONG_CORE_API                  __declspec(dllexport)
#define PONG_LEVEL_WIDTH               720//1020//720
#define PONG_LEVEL_HEIGHT              480//200//480

#define PONG_PLAYER_SPEED              5.0f
#define PONG_BALL_SPEED                5.0f

namespace pong
{
	
	class PONG_CORE_API GameState
	{
	public:
		static const int BALL_SIZE = 20;


		static const int PLAYER_PADDING = 30;
		static const int PLAYER_WIDTH = 20;
		static const int PLAYER_HEIGHT = 100;
		static const int PLAYER_1_X = PLAYER_PADDING;
		static const int PLAYER_2_X = PONG_LEVEL_WIDTH - PLAYER_PADDING - PLAYER_WIDTH;

		static const int TEXT_PIXEL_SIZE = 10;

		static const int BALL_COLOR   = 0xdddddd;
		static const int PLAYER_COLOR = 0xdddddd;
		static const int TEXT_COLOR   = 0x0055000;

		static const int LEVEL_CENTER = PONG_LEVEL_WIDTH / 2;
	private:
		float m_BallX;
		float m_BallY;
		float m_BallMotionX;
		float m_BallMotionY;
		float m_Player1Y;
		float m_Player2Y;

		int m_P1Score;
		int m_P2Score;
	public:
		GameState();

		void update(int player1Move, int player2Move);
	private:
		void movePlayers(int player1Move, int player2Move);
		void updateBall();
	public: 
		void draw(int buffer[PONG_LEVEL_WIDTH * PONG_LEVEL_HEIGHT]) const;
	};

}
