#include "NeuronNetwork.h"
#include <iostream>

#ifdef SHOW_HEATMAP
#	include <PongRenderer.h>
#	define IF_SHOW_HEATMAP(x) x
#else
#	define IF_SHOW_HEATMAP(x)
#endif

namespace cria
{
	NeuronNetwork::NeuronNetwork()
		: m_Bitmap(nullptr), 
		m_BitmapWidth(0), 
		m_BitmapHeight(0)
	{
		IF_SHOW_HEATMAP(m_Renderer = nullptr);
	}

	NeuronNetwork::~NeuronNetwork()
	{
		if (m_Bitmap)
			delete[] m_Bitmap;

		m_BitmapWidth = 0;
		m_BitmapHeight = 0;

		IF_SHOW_HEATMAP(if (m_Renderer) delete ((pong::Renderer*)m_Renderer));
	}

	void NeuronNetwork::processBitmap(int const* original, int width, int height)
	{
		if (m_Bitmap)
		{
			if (m_BitmapWidth != width || m_BitmapHeight != height)
			{
				delete[] m_Bitmap;
				IF_SHOW_HEATMAP(if (m_Renderer) delete ((pong::Renderer*)m_Renderer));
			}
		}
		if (!m_Bitmap)
		{
			m_Bitmap = new int[width * height];
			m_BitmapWidth = width;
			m_BitmapHeight = height;

			std::cout << "New Bitmap " << std::endl;
			IF_SHOW_HEATMAP(m_Renderer = new pong::Renderer(width, height, "NeuronNetwork: Heatmap"));
		}

		int subtractor = 0xff / CRIA_BITMAP_GENRATION_COUNT;
		int average;
		unsigned char const* c;
		for (int i = 0; i < width * height; i++)
		{
			
			c = (unsigned char const*)&original[i];
			average = c[1] + c[2] + c[3]; //c[0] is alpha and = 0
			average /= 3;
			
			if (average != 0)
			{
				m_Bitmap[i] = average;
				
			}
			else {
				if (m_Bitmap[i] != 0)
				{
					m_Bitmap[i] -= subtractor;
					if (m_Bitmap[i] < 0)
						m_Bitmap[i] = 0;
				}
			}
		}

		IF_SHOW_HEATMAP(((pong::Renderer*)m_Renderer)->loadPixels((char*)m_Bitmap));
		IF_SHOW_HEATMAP(((pong::Renderer*)m_Renderer)->drawTexture());
		IF_SHOW_HEATMAP(((pong::Renderer*)m_Renderer)->swapBuffers());
	}

	int const* NeuronNetwork::getBitmap() const
	{
		return m_Bitmap;
	}

	int NeuronNetwork::getBitmapWidth() const
	{
		return m_BitmapWidth;
	}

	int NeuronNetwork::getBitmapHeight() const
	{
		return m_BitmapHeight;
	}
}
