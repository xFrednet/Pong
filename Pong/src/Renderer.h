#pragma once

namespace pong
{
	class Renderer
	{
		int m_Width;
		int m_Height;
	public:
		Renderer(int width, int height);
		
		void render(char* chars);
	};
	
}