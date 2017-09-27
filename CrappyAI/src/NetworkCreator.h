#pragma once

#include "Crappy.h"
#include "NeuronNetwork.h"
#include "Neuron.h"
#include "CrappyAINodes.h"

#define SHUFFLE_LOOP_TIMES                  2

namespace cria
{
	enum CRAPPY_API CRIA_DATA_INDEX_STATE
	{
		CRIA_DATA_STATE_UNSET_VALUE    = 0x00,
		
		CRIA_DATA_STATE_UNUSED_INT     = 0x02,
		CRIA_DATA_STATE_UNUSED_FLOAT   = 0x04,

		CRIA_DATA_STATE_USED_INT       = 0x08,
		CRIA_DATA_STATE_USED_FLOAT     = 0x20,

		CRIA_DATA_STATE_LOOP_VALUE     = 0x40
	};

	struct CRAPPY_API NodeStackCreationState
	{
		CRIA_DATA_INDEX_STATE NeuronDataStates[CRIA_NEURON_DATA_LENGTH];
		int                   NeuronDataExInfo[CRIA_NEURON_DATA_LENGTH];
		
		uint                  NodeStackLength;
		
		uint                  SelectedNode;
		bool                  ReturnValueModified;

		NodeStackCreationState();

		int getUsableDataIndex();
		int getRandomIndexFromState(CRIA_DATA_INDEX_STATE state);
		int getDataTypeCount(CRIA_DATA_INDEX_STATE state);

		void useDataIndex(int index, CRIA_DATA_INDEX_STATE state, int exInfo = 0);
	};

	CRAPPY_API void ShuffleArray(int* integers, uint size);
	CRAPPY_API uint SelectIndexBasedOnChance(float weight[], uint size);

	CRAPPY_API Node* CreateIfActionNodes(NodeStackCreationState* cState, BaseIfNode* node);
	CRAPPY_API Node* CreateLoopNode(NodeStackCreationState* cState);
	CRAPPY_API Node* CreateGetPixelNode(NodeStackCreationState* cState);

}
