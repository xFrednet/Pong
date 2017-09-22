#pragma once

#include "Crappy.h"
#include "CrappyAINodes.h"

#define NEURON_DATA_LENGTH                       8

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
		NodeValue m_Data[NEURON_DATA_LENGTH];

		Node* m_Nodes;

		NeuronNetwork* m_Network;
	public:
		Neuron(Node* nodes, NeuronNetwork* network);

		NodeValue* getActiveData();

		NeuronNetwork* getNetwork();
		NeuronNetwork const* getNetwork() const;
	};
}