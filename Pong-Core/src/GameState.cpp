#include "GameState.h"
#include <cstring>
#include <corecrt_math.h>

#define TO_BUFFER_INDEX(x, y)           (x + y * PONG_LEVEL_WIDTH)
#define TEXT_PIXEL_SIZE                 (pong::GameState::TEXT_PIXEL_SIZE)
#define PONG_DIGIT_WIDTH                (TEXT_PIXEL_SIZE * 6)

namespace pong
{
	GameState::GameState()
		: m_BallX((PONG_LEVEL_WIDTH - BALL_SIZE) / 2.0f),
		m_BallY((PONG_LEVEL_HEIGHT - BALL_SIZE) / 2.0f),
		m_BallMotionX(PONG_BALL_X_SPEED), m_BallMotionY(0),
		m_Player1Y((PONG_LEVEL_HEIGHT - PLAYER_HEIGHT) / 2.0f), 
		m_Player2Y((PONG_LEVEL_HEIGHT - PLAYER_HEIGHT) / 2.0f),
		m_P1Score(0), m_P2Score(0)
	{

	}

	void GameState::update(int player1Move, int player2Move)
	{
		movePlayers(player1Move, player2Move);
		updateBall();
	}
	void GameState::movePlayers(int player1Move, int player2Move)
	{
		m_Player1Y += PONG_PLAYER_SPEED * player1Move;
		if (m_Player1Y < 0)
			m_Player1Y = 0;
		else if (m_Player1Y + PLAYER_HEIGHT >= PONG_LEVEL_HEIGHT)
			m_Player1Y = PONG_LEVEL_HEIGHT - PLAYER_HEIGHT - 1;

		m_Player2Y += PONG_PLAYER_SPEED * player2Move;
		if (m_Player2Y < 0)
			m_Player2Y = 0;
		else if (m_Player2Y + PLAYER_HEIGHT >= PONG_LEVEL_HEIGHT)
			m_Player2Y = PONG_LEVEL_HEIGHT - PLAYER_HEIGHT - 1;
	}
	void GameState::updateBall()
	{
		m_BallX += m_BallMotionX;
		m_BallY += m_BallMotionY;

		//top and bottom
		if (m_BallY < 0)
		{
			m_BallY *= -1;
			m_BallMotionY *= -1;
		} else if (m_BallY + BALL_SIZE >= PONG_LEVEL_HEIGHT)
		{
			m_BallY = PONG_LEVEL_HEIGHT - BALL_SIZE - ((m_BallY + BALL_SIZE) - (PONG_LEVEL_HEIGHT - 1));
			m_BallMotionY *= -1;
		}

		//score
		if (m_BallX < 0)
		{
			m_BallX = LEVEL_CENTER;
			m_P2Score++;
			m_BallMotionX = PONG_BALL_SPEED / -2;

			if (m_BallMotionY < -PONG_BALL_SPEED)
				m_BallMotionY = -PONG_BALL_SPEED;
			else if (m_BallMotionY > PONG_BALL_SPEED)
				m_BallMotionY = PONG_BALL_SPEED;
		} else if (m_BallX + BALL_SIZE >= PONG_LEVEL_WIDTH)
		{
			m_BallX = LEVEL_CENTER;
			m_P1Score++;
			m_BallMotionX = PONG_BALL_X_SPEED;

			if (m_BallMotionY < -PONG_BALL_SPEED)
				m_BallMotionY = -PONG_BALL_SPEED;
			else if (m_BallMotionY > PONG_BALL_SPEED)
				m_BallMotionY = PONG_BALL_SPEED;
		}

		// 1=>)    2=>(    3=>-    4=>_
		//  
		if (m_BallX < PLAYER_1_X + PLAYER_WIDTH && m_BallX + BALL_SIZE > PLAYER_1_X && // | |
			m_BallY + BALL_SIZE > m_Player1Y && m_BallY < m_Player1Y + PLAYER_HEIGHT)
		{
			float contact = (m_BallY + BALL_SIZE/2) - (m_Player1Y + PLAYER_HEIGHT / 2); //ranges from [ -PLAYER_HEIGHT/2 <-> PLAYER_HEIGHT/2 ]
			float relativeContact = contact / (PLAYER_HEIGHT / 2); // [-0.5 <-> 0.5]
			relativeContact *= (relativeContact < 0) ? -relativeContact : relativeContact;
			
			m_BallMotionY *= -1;
			m_BallMotionY = PONG_BALL_SPEED * relativeContact;
			m_BallMotionX *=  (m_BallMotionX < 0) ? -1 : 1;
		} else if (m_BallX + BALL_SIZE > PLAYER_2_X && m_BallX < PLAYER_2_X + PLAYER_WIDTH && // | |
			m_BallY + BALL_SIZE > m_Player2Y && m_BallY < m_Player2Y + PLAYER_HEIGHT)
		{
			float contact = (m_BallY + BALL_SIZE / 2) - (m_Player2Y + PLAYER_HEIGHT / 2); //ranges from [ -PLAYER_HEIGHT/2 <-> PLAYER_HEIGHT/2 ]
			float relativeContact = contact / (PLAYER_HEIGHT / 2); // [-0.5 <-> 0.5]
			relativeContact *= (relativeContact < 0) ? -relativeContact : relativeContact;

			m_BallMotionY *= -1;
			m_BallMotionY = PONG_BALL_SPEED * relativeContact;
			m_BallMotionX *= (m_BallMotionX < 0) ? 1 : -1;
		}
	}

	inline void drawRectangle(int* buffer, int xx, int yy, int width, int height, int color)
	{
		int xi;
		int yi;
		int ya;
		int xa;
		for (yi = 0; yi < height; yi++) {
			ya = yy + yi;
			if (ya < 0 || ya >= PONG_LEVEL_HEIGHT)
				continue;

			for (xi = 0; xi < width; xi++) {
				xa = xx + xi;
				if (xa < 0 || xa >= PONG_LEVEL_WIDTH)
					continue;

				buffer[TO_BUFFER_INDEX(xa, ya)] = color;
			}
		}
	}
	inline void drawDigitDisplay(int* buffer, int xx, int yy, char draw, int color)
	{
		// draw : bits
		//
		//  #0#
		// #   #
		// 1   2
		// #   #
		//  #3#
		// #   #
		// 4   5
		// #   #
		//  #6#
		if (draw & 0x01) // 0000 0001
			drawRectangle(buffer, xx + TEXT_PIXEL_SIZE		, yy						, TEXT_PIXEL_SIZE * 3	, TEXT_PIXEL_SIZE		, color);
		if (draw & 0x02) // 0000 0010
			drawRectangle(buffer, xx						, yy + TEXT_PIXEL_SIZE		, TEXT_PIXEL_SIZE		, TEXT_PIXEL_SIZE * 3	, color);
		if (draw & 0x04) // 0000 0100
			drawRectangle(buffer, xx + TEXT_PIXEL_SIZE * 4	, yy + TEXT_PIXEL_SIZE		, TEXT_PIXEL_SIZE		, TEXT_PIXEL_SIZE * 3	, color);
		if (draw & 0x08) // 0000 1000
			drawRectangle(buffer, xx + TEXT_PIXEL_SIZE		, yy + TEXT_PIXEL_SIZE * 4	, TEXT_PIXEL_SIZE * 3	, TEXT_PIXEL_SIZE		, color);


		if (draw & 0x10) // 0001 0000
			drawRectangle(buffer, xx						, yy + TEXT_PIXEL_SIZE * 5	, TEXT_PIXEL_SIZE		, TEXT_PIXEL_SIZE * 3	, color);
		if (draw & 0x20) // 0010 0000
			drawRectangle(buffer, xx + TEXT_PIXEL_SIZE * 4	, yy + TEXT_PIXEL_SIZE * 5	, TEXT_PIXEL_SIZE		, TEXT_PIXEL_SIZE * 3	, color);
		if (draw & 0x40) // 0100 0000
			drawRectangle(buffer, xx + TEXT_PIXEL_SIZE		, yy + TEXT_PIXEL_SIZE * 8	, TEXT_PIXEL_SIZE * 3	, TEXT_PIXEL_SIZE		, color);
	}
	inline void drawNumber(int* buffer, int xx, int yy, int number, int color)
	{
		char draw = 0x80;
		if (number < 0)
		{
			draw = 0x08; // 0000 1000 -> '-'
			number *= -1;
			drawDigitDisplay(buffer, xx, yy, draw, color);

			xx += PONG_DIGIT_WIDTH;
		}

		if (number >= 10)
		{
			                                                  // number = 87932
			int log = (int)log10f((float)number);             // log -> 5
			int drawNumb = (int)floor(number / pow(10, log)); // number / pow(5, 10) -> 8
			
			drawNumber(buffer, xx + PONG_DIGIT_WIDTH, yy, number % ((int)pow(10, log) * drawNumb), color);
			number = drawNumb;
		}

		switch (number)
		{
		case 0:
			draw = 0x77; //0111 0111
			break;
		case 1:
			draw = 0x24; //0010 0100
			break;
		case 2:
			draw = 0x5D; //0101 1101
			break;
		case 3:
			draw = 0x6D; //0110 1101
			break;
		case 4:
			draw = 0x2E; //0010 1110
			break;
		case 5:
			draw = 0x6B; //0110 1011
			break;
		case 6:
			draw = 0x7B; //0111 1011
			break;
		case 7:
			draw = 0x25; //0010 0101
			break;
		case 8:
			draw = 0x7F; //0111 1111
			break;
		case 9:
			draw = 0x6F; //0110 1111
			break;
		default:
			break;
		}

		drawDigitDisplay(buffer, xx, yy, draw, color);
	}
	void GameState::draw(int buffer[PONG_LEVEL_WIDTH * PONG_LEVEL_HEIGHT]) const
	{
		memset(buffer, 0, PONG_LEVEL_WIDTH * PONG_LEVEL_HEIGHT * 4);
		for (int c = 0, i = 0; i < PONG_LEVEL_HEIGHT; i++, c++, c %= 20)
		{
			if (c < 10)
				buffer[TO_BUFFER_INDEX(LEVEL_CENTER, i)] = TEXT_COLOR;
		}
		drawNumber(buffer, LEVEL_CENTER - PONG_DIGIT_WIDTH * 3, 10, m_P1Score, TEXT_COLOR);
		drawNumber(buffer, LEVEL_CENTER + PONG_DIGIT_WIDTH * 2, 10, m_P2Score, TEXT_COLOR);
		
		//ball
		drawRectangle(buffer, (int)m_BallX, (int)m_BallY, BALL_SIZE, BALL_SIZE, BALL_COLOR);

		//player
		drawRectangle(buffer, (int)PLAYER_1_X, (int)m_Player1Y, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_COLOR);
		drawRectangle(buffer, (int)PLAYER_2_X, (int)m_Player2Y, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_COLOR);

	}
}
