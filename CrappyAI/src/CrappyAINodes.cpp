#include "CrappyAINodes.h"

#include "NeuronNetwork.h"
#include "Neuron.h"

#include <cassert>
#include <cmath>
#include <iostream>

// ####################################
// # NodeValue #
// ####################################
namespace cria
{
	int int_null_value_ = 0;
	float float_null_value_ = 0.0f;

	NodeValue::NodeValue()
		: NodeValue(INT_VALUE, 0)
	{
	}
	NodeValue::NodeValue(ValueType type, int value)
	{
		reset(type, value);
	}
	NodeValue::NodeValue(ValueType type, float value)
	{
		reset(type, value);
	}

	void NodeValue::reset(ValueType type, int value)
	{
		if (type == INDEX_INT_VALUE || type == INDEX_FLOAT_VALUE) {
			assert(value >= 0);
			assert(value < NEURON_DATA_LENGTH);

			if (value < 0)
				value = 0;
			else
				value = NEURON_DATA_LENGTH - 1;

			m_Type = type;
			m_IndexValue = value;

		}
		else if (type == INT_VALUE) {
			m_Type = type;
			m_IntValue = value;
		} 
		else {
			assert(false, "The entered type is unknown.");
			m_Type = INT_VALUE;
			m_IntValue = 0;
		}
	}
	void NodeValue::reset(ValueType type, float value)
	{
		if (type == FLOAT_VALUE) {
			m_Type = type;
			m_FloatValue = value;
		}
		else {
			assert(type == FLOAT_VALUE);
			m_Type = FLOAT_VALUE;
			m_FloatValue = 0;
		}
	}

	bool NodeValue::isInt() const
	{
		return (m_Type == INT_VALUE || m_Type == INDEX_INT_VALUE);
	}
	bool NodeValue::isFloat() const
	{
		return m_Type == FLOAT_VALUE || m_Type == INDEX_FLOAT_VALUE;
	}

	bool NodeValue::isIndex() const
	{
		return m_Type == INDEX_INT_VALUE || m_Type == INDEX_FLOAT_VALUE;
	}

	NodeValue::ValueType NodeValue::getType() const
	{
		return m_Type;
	}

	int& NodeValue::getIntValue(Neuron* neuron)
	{
		if (m_Type == INT_VALUE)
			return m_IntValue;
		
		if (m_Type == INDEX_INT_VALUE && neuron)
			if (neuron->m_Data[m_IndexValue].m_Type == INT_VALUE)
				return neuron->m_Data[m_IndexValue].getIntValue(neuron);

		return int_null_value_;
	}
	float& NodeValue::getFloatValue(Neuron* neuron)
	{
		if (m_Type == FLOAT_VALUE)
			return m_FloatValue;

		if (m_Type == INDEX_FLOAT_VALUE && neuron)
			if (neuron->m_Data[m_IndexValue].m_Type == FLOAT_VALUE)
				return neuron->m_Data[m_IndexValue].getFloatValue(neuron);

		return float_null_value_;
	}

	float NodeValue::getAsFloat(Neuron* neuron)
	{
		if (isFloat())
			return getFloatValue(neuron);
		
		return (float)getIntValue(neuron);
	}
	int NodeValue::getAsInt(Neuron* neuron)
	{
		if (isInt())
			return getIntValue(neuron);

		return (int)floor(getFloatValue(neuron));
	}

	void NodeValue::set(int i, Neuron* neuron)
	{
		if (m_Type == INT_VALUE)
			m_IntValue = i;
		else if (m_Type == FLOAT_VALUE)
			m_FloatValue = (float)i;
		else if (isIndex())
			neuron->m_Data[m_IndexValue].set(i, neuron);
	}
	void NodeValue::set(float f, Neuron* neuron)
	{
		if (m_Type == FLOAT_VALUE)
			m_FloatValue = f;
		else if (m_Type == INT_VALUE)
			m_IntValue = (int)f;
		else if (isIndex())
			neuron->m_Data[m_IndexValue].set(f, neuron);
	}
}

// ##############################################
// # Node #
// ##############################################
namespace cria {
	node_return Node::callStack(Neuron* neuron)
	{
		Node* n = this;
		while (n)
		{
			n->beCrappy(neuron);
			n = n->m_Next;
		}

		return 0;
	}
}

// ##############################################
// # TwoValueNode #
// ##############################################
namespace cria {
	TwoValueNode::TwoValueNode(const NodeValue& var1, const NodeValue& var2)
		:m_Var1(var1), m_Var2(var2)
	{
	}

	node_return TwoValueNode::beCrappy(Neuron* neuron)
	{
		assert(neuron);

		if (m_Var1.isInt() && m_Var2.isInt()) {
			int a = m_Var1.getIntValue(neuron);
			int b = m_Var2.getIntValue(neuron);

			return beCrappyInt(a, b, neuron);
		}
		
		if (m_Var1.isFloat() && m_Var2.isFloat())
		{
			float a = m_Var1.getAsFloat(neuron);
			float b = m_Var2.getAsFloat(neuron);

			return  beCrappyFloat(a, b, neuron);
		}
		
		return 0;
	}
}
// ##############################################
// # BaseIfNode #
// ##############################################
namespace cria
{
	BaseIfNode::BaseIfNode(const NodeValue& var1, const NodeValue& var2, Node* trueAction, Node* falseAction)
		: TwoValueNode(var1, var2),
		m_ActionOnTrue(trueAction),
		m_ActionOnFalse(falseAction)
	{
	}

	node_return BaseIfNode::activateTrueNode(Neuron* neuron)
	{
		if (m_ActionOnTrue)
			return m_ActionOnTrue->callStack(neuron);

		return 0;
	}
	node_return BaseIfNode::activateFalseNode(Neuron* neuron)
	{
		if (m_ActionOnFalse)
			return m_ActionOnFalse->callStack(neuron);

		return 0;
	}
}

// ##############################################
// # AddNode #
// ##############################################
namespace cria {
	AddNode::AddNode(const NodeValue& var1, const NodeValue& var2)
		: TwoValueNode(var1, var2)
	{
	}

	node_return AddNode::beCrappyInt(int a, int b, Neuron* neuron)
	{
		neuron->getActiveData()->reset(NodeValue::INT_VALUE, a + b);
		return 0;
	}
	node_return AddNode::beCrappyFloat(float a, float b, Neuron* neuron)
	{
		neuron->getActiveData()->reset(NodeValue::FLOAT_VALUE, a + b);
		return 0;
	}
}
// ##############################################
// # SubtractNode #
// ##############################################
namespace cria {
	SubtractNode::SubtractNode(const NodeValue& var1, const NodeValue& var2)
		: TwoValueNode(var1, var2)
	{
	}

	node_return SubtractNode::beCrappyInt(int a, int b, Neuron* neuron)
	{
		neuron->getActiveData()->reset(NodeValue::INT_VALUE, a + b);
		return 0;
	}
	node_return SubtractNode::beCrappyFloat(float a, float b, Neuron* neuron)
	{
		neuron->getActiveData()->reset(NodeValue::FLOAT_VALUE, a + b);
		return 0;
	}

}
// ##############################################
// # MultiplyNode #
// ##############################################
namespace cria {
	MultiplyNode::MultiplyNode(const NodeValue& var1, const NodeValue& var2)
		: TwoValueNode(var1, var2)
	{
	}

	node_return MultiplyNode::beCrappyInt(int a, int b, Neuron* neuron)
	{
		neuron->getActiveData()->reset(NodeValue::INT_VALUE, a * b);
		return 0;
	}
	node_return MultiplyNode::beCrappyFloat(float a, float b, Neuron* neuron)
	{
		neuron->getActiveData()->reset(NodeValue::FLOAT_VALUE, a * b);
		return 0;
	}

}
// ##############################################
// # DivideNode #
// ##############################################
namespace cria {
	DivideNode::DivideNode(const NodeValue& var1, const NodeValue& var2)
		: TwoValueNode(var1, var2)
	{
	}

	node_return DivideNode::beCrappyInt(int a, int b, Neuron* neuron)
	{
		neuron->getActiveData()->reset(NodeValue::INT_VALUE, (a != 0 && b != 0) ? (a / b) : 0);
		return 0;
	}
	node_return DivideNode::beCrappyFloat(float a, float b, Neuron* neuron)
	{
		neuron->getActiveData()->reset(NodeValue::FLOAT_VALUE, (a != 0 && b != 0) ? (a / b) : 0);
		return 0;
	}

}

// ##############################################
// # SetActiveSlotNode #
// ##############################################
namespace cria {
	SetActiveSlotNode::SetActiveSlotNode(const NodeValue& slot)
		: m_Slot(slot)
	{
	}

	node_return SetActiveSlotNode::beCrappy(Neuron* neuron)
	{
		int i = m_Slot.getAsInt(neuron);
		if (i < 0)
			i = 0;
		else if (i >= NEURON_DATA_LENGTH)
			i = NEURON_DATA_LENGTH - 1;

		neuron->m_ActiveDataIndex = i;

		return 0;
	}
}

// ##############################################
// # IfEqualNode #
// ##############################################
namespace cria {
	IfEqualNode::IfEqualNode(const NodeValue& var1, const NodeValue& var2, Node* trueAction, Node* falseAction)
		: BaseIfNode(var1, var2, trueAction, falseAction)
	{
	}

	node_return IfEqualNode::beCrappyInt(int a, int b, Neuron* neuron)
	{
		if (a == b)
			return activateTrueNode(neuron);
		else
			return activateFalseNode(neuron);
	}
	node_return IfEqualNode::beCrappyFloat(float a, float b, Neuron* neuron)
	{
		if (a == b)
			return activateTrueNode(neuron);
		else
			return activateFalseNode(neuron);
	}
}
// ##############################################
// # IfLessNode #
// ##############################################
namespace cria {
	IfLessNode::IfLessNode(const NodeValue& var1, const NodeValue& var2, Node* trueAction, Node* falseAction)
		: BaseIfNode(var1, var2, trueAction, falseAction)
	{
	}

	node_return IfLessNode::beCrappyInt(int a, int b, Neuron* neuron)
	{
		if (a < b)
			return activateTrueNode(neuron);
		else
			return activateFalseNode(neuron);
	}
	node_return IfLessNode::beCrappyFloat(float a, float b, Neuron* neuron)
	{
		if (a < b)
			return activateTrueNode(neuron);
		else
			return activateFalseNode(neuron);
	}
}
// ##############################################
// # BreakIfEqualNode #
// ##############################################
namespace cria {
	BreakIfEqualNode::BreakIfEqualNode(const NodeValue& var1, const NodeValue& var2)
		: TwoValueNode(var1, var2)
	{
	}

	node_return BreakIfEqualNode::beCrappyInt(int a, int b, Neuron* neuron)
	{
		if (a == b)
			return true;
		else 
			return false;
	}
	node_return BreakIfEqualNode::beCrappyFloat(float a, float b, Neuron* neuron)
	{
		if (a == b)
			return true;
		else
			return false;
	}
}
// ##############################################
// # BreakIfLessNode #
// ##############################################
namespace cria {
	BreakIfLessNode::BreakIfLessNode(const NodeValue& var1, const NodeValue& var2)
		: TwoValueNode(var1, var2)
	{
	}

	node_return BreakIfLessNode::beCrappyInt(int a, int b, Neuron* neuron)
	{
		if (a < b)
			return true;
		else
			return false;
	}
	node_return BreakIfLessNode::beCrappyFloat(float a, float b, Neuron* neuron)
	{
		if (a < b)
			return true;
		else
			return false;
	}
}

// ##############################################
// # ForNode #
// ##############################################
namespace cria {
	ForNode::ForNode(const NodeValue& start, const NodeValue& limit, const NodeValue& incrementer, const NodeValue& indexWriteLoc, Node* actionNodes)
		: m_Start(start),
		m_Limit(limit),
		m_Incrementer(incrementer),
		m_IndexWriteLoc(indexWriteLoc),
		m_ActionNodes(actionNodes)
	{
		if (m_IndexWriteLoc.isIndex())
			std::cout << "ForNode the index write location should be an data index" << std::endl;
	}

	node_return ForNode::beCrappy(Neuron* neuron)
	{
		if (!m_ActionNodes)
			return 0;

		Node* n;
		int limit = m_Limit.getAsInt(neuron);
		for (int i = m_Start.getAsInt(neuron); i < limit; i += m_Incrementer.getAsInt(neuron))
		{
			m_IndexWriteLoc.set(i, neuron);

			n = m_ActionNodes;
			while (n) {
				if (n->getType() == CRIA_NODE_BREAK_IF_EQUAL || n->getType() == CRIA_NODE_BREAK_IF_LESS) {
					if (n->beCrappy(neuron))
						return 0;
				} else
				{
					n->beCrappy(neuron);
				}
				n = n->m_Next;
			}
		}
		return 0;
	}
}

// ##############################################
// # GetPixelNode #
// ##############################################
namespace cria {
	GetPixelNode::GetPixelNode(NodeValue testX, NodeValue testY)
		: m_TestX(testX),
		m_TestY(testY)
	{
	}

	node_return GetPixelNode::beCrappy(Neuron* neuron)
	{
		NeuronNetwork* network = neuron->getNetwork();
		int x = m_TestX.getAsInt(neuron);
		int y = m_TestY.getAsInt(neuron);

		if (x < 0 || x >= network->getBitmapWidth() || y < 0 || y >= network->getBitmapHeight())
			return 0;

		int p = network->getBitmap()[x + y * network->getBitmapWidth()];
		neuron->getActiveData()->reset(NodeValue::INT_VALUE, p);
		return p;
	}
}
