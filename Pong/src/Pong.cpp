#include "Pong.h"
#include <cmath>
#include <cstring>
#include <string>

namespace pong
{
	Pong::Pong(Renderer* renderer)
		: m_Renderer(renderer)
	{
		m_PlayerYPos[PONG_PLAYER_1] = PONG_LEVEL_HEIGHT / 2.0f - PONG_PLAYER_SIZE / 2.0f;
		m_PlayerYPos[PONG_PLAYER_2] = PONG_LEVEL_HEIGHT / 2.0f - PONG_PLAYER_SIZE / 2.0f;

		m_BallX = (float)PONG_PLAYER_1_X + 1;
		m_BallY = m_PlayerYPos[PONG_PLAYER_1];
		m_BallXM = -0.25f;
		m_BallYM = -0.20f;
	}

	void Pong::movePlayer(int player, int move)
	{
		m_PlayerYPos[player] += (float)move * PONG_PLAYER_SPEED;
		if (m_PlayerYPos[player] < 0)
			m_PlayerYPos[player] = 0;
		if (m_PlayerYPos[player] + PONG_PLAYER_SIZE >= PONG_LEVEL_HEIGHT)
			m_PlayerYPos[player] = PONG_LEVEL_HEIGHT - PONG_PLAYER_SIZE - 1;
	}
	int c1, c2;
	void Pong::moveBall()
	{
		float newBallX = m_BallX + m_BallXM;
		float newBallY = m_BallY + m_BallYM;
		
		if (newBallY < 0)
		{
			m_BallY = (newBallY);
			m_BallY *= -1;
			m_BallYM *= -1;
		} else if (newBallY >= PONG_LEVEL_HEIGHT)
		{
			m_BallY = PONG_LEVEL_HEIGHT - (newBallY - PONG_LEVEL_HEIGHT);
			m_BallYM *= -1;
		} else
		{
			m_BallY = newBallY;
		}

		/*int bx = (int)floorf(newBallX);
		int by = (int)floorf(newBallY);*/
		if (newBallX == PONG_PLAYER_1_X)
		{
			if (newBallY >= m_PlayerYPos[PONG_PLAYER_1] && newBallY <= m_PlayerYPos[PONG_PLAYER_1] + PONG_PLAYER_SIZE)
			{
				float contact = (int)floor(newBallY - m_PlayerYPos[PONG_PLAYER_1]) - ((PONG_PLAYER_SIZE / 2) - 1);
				m_BallXM *= -1;
				m_BallYM += (PONG_MAX_BALL_BOUNCE_SPEED_GAIN / PONG_PLAYER_SIZE) * contact * 2;
				c1 = contact;
			}
		} else if (newBallX == PONG_PLAYER_2_X)
		{
			if (newBallY >= m_PlayerYPos[PONG_PLAYER_2] && newBallY <= m_PlayerYPos[PONG_PLAYER_2] + PONG_PLAYER_SIZE) {
				float contact = (int)floor(newBallY - m_PlayerYPos[PONG_PLAYER_2]) - ((PONG_PLAYER_SIZE / 2) - 1);
				m_BallXM *= -1;
				m_BallYM += (PONG_MAX_BALL_BOUNCE_SPEED_GAIN / PONG_PLAYER_SIZE) * contact * 2;
				c2 = contact;
			}
		} else
		{
			m_BallX += m_BallXM;
		}
		
	}

	char charMap[PONG_LEVEL_WIDTH * PONG_LEVEL_HEIGHT];
	void Pong::draw()
	{
		if (!m_Renderer)
			return;

		int x, y;
		memset(charMap, ' ', PONG_LEVEL_WIDTH * PONG_LEVEL_HEIGHT);

		x = (int)roundf(m_BallX);
		y = (int)roundf(m_BallY);
		charMap[x + y * PONG_LEVEL_WIDTH] = 'O';

		for (int i = 0; i < PONG_PLAYER_SIZE; i++)
		{
			x = PONG_PLAYER_1_X;
			y = (int)floor(m_PlayerYPos[PONG_PLAYER_1]) + i;
			charMap[x + y * PONG_LEVEL_WIDTH] = '#';

			x = PONG_PLAYER_2_X;
			y = (int)floor(m_PlayerYPos[PONG_PLAYER_2]) + i;
			charMap[x + y * PONG_LEVEL_WIDTH] = '#';
		}

		std::string s = std::to_string(c2);
		memcpy(&charMap[PONG_LEVEL_WIDTH - 4], s.c_str(), s.length());
		s = std::to_string(c1);
		memcpy(&charMap[3], s.c_str(), s.length());

		m_Renderer->render(charMap);
	}

	void Pong::update(int player1Move, int player2Move)
	{
		//move Players
		movePlayer(PONG_PLAYER_1, player1Move);
		movePlayer(PONG_PLAYER_2, player2Move);

		moveBall();
		draw();
	}
}

