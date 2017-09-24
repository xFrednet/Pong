#pragma once

#include "Crappy.h"
#include "NeuronNetwork.h"
#include "Neuron.h"
#include "CrappyAINodes.h"

namespace cria
{
	enum CRAPPY_API CRIA_DATA_INDEX_STATE
	{
		CRIA_DATA_STATE_UNSET_VALUE,
		
		CRIA_DATA_STATE_UNUSED_VALUE,
		CRIA_DATA_STATE_USED_VALUE,

		CRIA_DATA_STATE_LOOP_VALUE
	};

	struct CRAPPY_API NodeStackCreationState
	{
		CRIA_DATA_INDEX_STATE NeuronDataStates[NEURON_DATA_LENGTH];
		uint                  NodeStackLength;
		
		uint                  SelectedNode;
		bool                  ReturnValueModified;

		NodeStackCreationState()
			: NodeStackLength(0),
			SelectedNode(0),
			ReturnValueModified(false)
		{
			NeuronDataStates[0] = CRIA_DATA_STATE_UNUSED_VALUE;

			for (uint i = 1; i < NEURON_DATA_LENGTH; i++)
			{
				NeuronDataStates[i] = CRIA_DATA_STATE_UNSET_VALUE;
			}
		}
	};

	CRAPPY_API uint SelectIndexBasedOnChance(float weight[], uint size);

	CRAPPY_API Node* CreateIfActionNodes(NodeStackCreationState* cState, BaseIfNode* node);
	
	CRAPPY_API Node* CreateLoopNode(NodeStackCreationState* cState);
}
