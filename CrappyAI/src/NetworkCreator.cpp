#include "NetworkCreator.h"
#include <cstdlib>

namespace cria
{
	uint SelectIndexBasedOnChance(float weight[], uint size)
	{
		uint i;
		float total = 0;

		//total
		for (i = 0; i < size; i++)
			total += weight[i];

		//adjusting weights
		for (i = 0; i < size; i++)
			weight[i] /= total;

		float randValue = (float)rand() / (float)RAND_MAX;

		//return
		if (randValue < weight[0])
			return 0;
		for (i = 1; i < size; i++)
			if (randValue >= weight[i - 1] && randValue < weight[i])
				return i;

		return size - 1;
	}
}
