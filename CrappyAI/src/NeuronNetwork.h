#pragma once

#define CRIA_BITMAP_GENRATION_COUNT               4

#include "Crappy.h"
#include "Neuron.h"

namespace cria
{
	class CRAPPY_API NeuronNetwork
	{
	private:
		//every bitmap value is between 0xff and 0.
		// when a new bitmap is submitted it will decrease every value by (0xff / CRIA_BITMAP_GENRATION_COUNT) this
		// creates a heat map for farther processing.
		int* m_Bitmap;
		int m_BitmapWidth;
		int m_BitmapHeight;
		
		//debug value
		void* m_Renderer;
	public:
		NeuronNetwork();
		~NeuronNetwork();

		void processBitmap(int const* original, int width, int height);

		int const* getBitmap() const;
		int getBitmapWidth() const;
		int getBitmapHeight() const;
	};
}
