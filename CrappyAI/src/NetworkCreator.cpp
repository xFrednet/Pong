#include "NetworkCreator.h"
#include <cstdlib>

#define min(x, y)                       (((x) < (y)) ? (x) : (y))
#define RANDF_0_TO_1                    ((float)rand() / (float)RAND_MAX)
#define NOTEVALUE_TYPE_FROM_DATA_STATE_I(x) \
	(\
		/*if*/(x == CRIA_DATA_STATE_UNUSED_INT || x == CRIA_DATA_STATE_USED_INT || x == CRIA_DATA_STATE_LOOP_VALUE) ? (\
			cria::NodeValue::INDEX_INT_VALUE\
		) : ( /*else if*/(x == CRIA_DATA_STATE_UNUSED_FLOAT || x == CRIA_DATA_STATE_USED_FLOAT) ? (\
			cria::NodeValue::INDEX_FLOAT_VALUE\
		) : /*else*/\
			cria::NodeValue::INDEX_INT_VALUE)\
	)
//
// NodeStackCreationState
//
namespace cria {
	NodeStackCreationState::NodeStackCreationState()
		: NodeStackLength(0),
		SelectedNode(0),
		ReturnValueModified(false)
	{
		NeuronDataStates[0] = CRIA_DATA_STATE_UNUSED_FLOAT;

		for (uint i = 1; i < CRIA_NEURON_DATA_LENGTH; i++) {
			NeuronDataStates[i] = CRIA_DATA_STATE_UNSET_VALUE;
			NeuronDataExInfo[i] = 0;
		}
	}

	int NodeStackCreationState::getUsableDataIndex()
	{
		for (int i = 1; i < CRIA_NEURON_DATA_LENGTH; i++)
		{
			if (NeuronDataStates[i] == CRIA_DATA_STATE_UNSET_VALUE)
				return i;
		}

		for (int i = 0; i < CRIA_NEURON_DATA_LENGTH; i++)
		{
			if (NeuronDataStates[CRIA_NEURON_DATA_LENGTH - 1 - i] == CRIA_DATA_STATE_UNSET_VALUE)
				return CRIA_NEURON_DATA_LENGTH - 1 - i;
		}

		CRIA_DATA_INDEX_STATE state = (CRIA_DATA_INDEX_STATE)(CRIA_DATA_STATE_USED_FLOAT | CRIA_DATA_STATE_USED_INT);
		return getRandomIndexFromState(state);
	}
	int NodeStackCreationState::getRandomIndexFromState(CRIA_DATA_INDEX_STATE state)
	{
		//setting up the indices
		static int indices[CRIA_NEURON_DATA_LENGTH];
		if (indices[0] == indices[1])
			for (int i = 0; i < CRIA_NEURON_DATA_LENGTH; i++) 
				indices[i] = i;

		//shuffle
		ShuffleArray(indices, CRIA_NEURON_DATA_LENGTH);

		//find node
		for (uint i = 0; i < CRIA_NEURON_DATA_LENGTH; i++)
		{
			if (NeuronDataStates[indices[i]] & state)
				return indices[i];
		}

		return -1;
	}

	void NodeStackCreationState::useDataIndex(int index, CRIA_DATA_INDEX_STATE state, int exInfo)
	{
		if (index < 0 || index >= CRIA_NEURON_DATA_LENGTH)
			return;

		NeuronDataStates[index] = state;
		NeuronDataExInfo[index] = exInfo;
	}

}
namespace cria {
	void ShuffleArray(int* integers, uint size)
	{
		int copyValue;
		int randIndex1;
		int randIndex2;
		for (uint shuffleTimes = 0; shuffleTimes < SHUFFLE_LOOP_TIMES; shuffleTimes++)
		{
			for (uint i = 0; i < size; i++)
			{
				randIndex1 = rand() % size;
				randIndex2 = rand() % size;

				copyValue            = integers[randIndex1];
				integers[randIndex1] = integers[randIndex2];
				integers[randIndex2] = copyValue;
			}
		}
	}
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

	Node* CreateLoopNode(NodeStackCreationState* cState)
	{
		NodeValue start;
		NodeValue limit;
		NodeValue incrementer; //NoteValue::INT_VALUE
		NodeValue writeIndex;  //NoteValue::INT_VALUE
		// Value creation
		{
			CRIA_DATA_INDEX_STATE states[] = { CRIA_DATA_STATE_UNUSED_INT, CRIA_DATA_STATE_UNUSED_FLOAT, CRIA_DATA_STATE_USED_INT, CRIA_DATA_STATE_USED_FLOAT};
			int index = 0;
			int writeDataIndex;
			int count = 0;
			int possibilities[CRIA_NEURON_DATA_LENGTH];
			
			//write index
			writeDataIndex = cState->getUsableDataIndex();
			cState->useDataIndex(writeDataIndex, CRIA_DATA_STATE_LOOP_VALUE, 0);
			writeIndex.reset(NodeValue::INDEX_INT_VALUE, writeDataIndex);
			
			index = 0;
			//fill list
			for (int i = 0; i < (sizeof(states) / sizeof(CRIA_DATA_INDEX_STATE)); i++)
			{
				if (i == 0) 
					count += cState->getDataTypeCount(states[i]);
				else 
					count += cState->getDataTypeCount(states[i]) / 2;

				for (; index < min(count, CRIA_NEURON_DATA_LENGTH); index++)
					possibilities[index] = states[i];
			}
			for (; index < CRIA_NEURON_DATA_LENGTH; index++)
				possibilities[index] = -1;
			//shuffle
			ShuffleArray(possibilities, CRIA_NEURON_DATA_LENGTH);
			//select
			if (possibilities[0] == -1)
				start.reset(NodeValue::INT_VALUE, 0);
			else
				start.reset(NOTEVALUE_TYPE_FROM_DATA_STATE_I(possibilities[0]), cState->getRandomIndexFromState((CRIA_DATA_INDEX_STATE)possibilities[0]));

			if (possibilities[1] == -1)
				limit.reset(NodeValue::INT_VALUE, 0);
			else
				limit.reset(NOTEVALUE_TYPE_FROM_DATA_STATE_I(possibilities[1]), cState->getRandomIndexFromState((CRIA_DATA_INDEX_STATE)possibilities[1]));

			//creating "const" ints
			if (RANDF_0_TO_1 < 0.8f)
				incrementer.reset(NodeValue::INT_VALUE, 1);
			//else 
				//incrementer.reset(NodeValue::INT_VALUE, randInt())
		}
	}
}
