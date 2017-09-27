#pragma once

#include "Crappy.h"
#include "CrappyAINodes.h"

#define CRIA_NEURON_DATA_LENGTH                       8

namespace cria
{
	class NeuronNetwork;

	class CRAPPY_API Neuron
	{
	private:
		friend class NodeValue;
		friend class SetActiveSlotNode;
	private:
		int m_ActiveDataIndex = 0;
		NodeValue m_Data[CRIA_NEURON_DATA_LENGTH];

		Node* m_Nodes;

		NeuronNetwork* m_Network;
	public:
		Neuron(Node* nodes, NeuronNetwork* network);

		NodeValue* getActiveData();

		NeuronNetwork* getNetwork();
		NeuronNetwork const* getNetwork() const;
	};
}