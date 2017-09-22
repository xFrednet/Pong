#include "Neuron.h"

namespace cria
{
	Neuron::Neuron(Node* nodes, NeuronNetwork* network)
		: m_ActiveDataIndex(0),
		m_Nodes(nodes),
		m_Network(network)
	{
	}

	NodeValue* Neuron::getActiveData()
	{
		if (m_ActiveDataIndex > 0 || m_ActiveDataIndex < NEURON_DATA_LENGTH)
			return &m_Data[m_ActiveDataIndex];

		return nullptr;
	}

	NeuronNetwork* Neuron::getNetwork()
	{
		return m_Network;
	}
	NeuronNetwork const* Neuron::getNetwork() const
	{
		return m_Network;
	}
}
