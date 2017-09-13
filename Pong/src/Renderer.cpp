#include "Renderer.h"

#include <Windows.h>
#include <iostream>

namespace pong
{
	Renderer::Renderer(int width, int height)
		: m_Width(width),
		m_Height(height)
	{
		HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
		height += 1;
		//
		// resizing the console
		//
		{
			CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
			if (!GetConsoleScreenBufferInfo(hCon, &bufferInfo))
				std::cout << "Unable to retrieve screen buffer info." << std::endl;

			SMALL_RECT& winInfo = bufferInfo.srWindow;
			COORD windowSize = { winInfo.Right - winInfo.Left + 1, winInfo.Bottom - winInfo.Top + 1 };

			if (windowSize.X > (SHORT)width || windowSize.Y > (SHORT)height) {
				// window size needs to be adjusted before the buffer size can be reduced.
				SMALL_RECT info =
				{
					0,
					0,
					((SHORT)width < windowSize.X) ? (SHORT)(width - 1) : (windowSize.X - 1),
					((SHORT)width < windowSize.Y) ? (SHORT)(width - 1) : (windowSize.Y - 1)
				};

				if (!SetConsoleWindowInfo(hCon, TRUE, &info))
					std::cout << "Unable to resize window before resizing buffer." << std::endl;
			}
			//
			// resizing the Buffer
			//
			COORD size = { (SHORT)width, (SHORT)height };
			if (!SetConsoleScreenBufferSize(hCon, size))
				std::cout << "Unable to resize screen buffer." << std::endl;

			SMALL_RECT info = { 0, 0, (SHORT)width - 1, (SHORT)height - 1 };
			if (!SetConsoleWindowInfo(hCon, TRUE, &info))
				std::cout << "Unable to resize window after resizing buffer." << std::endl;
		}
	}

	void Renderer::render(char* chars)
	{
		HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD pos = { 0, 0 };
		DWORD written;

		SetConsoleCursorPosition(hCon, pos);
		WriteConsole(hCon, chars, m_Width * m_Height, &written, NULL);
	}
}
