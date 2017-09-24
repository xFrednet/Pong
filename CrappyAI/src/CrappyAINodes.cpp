#include "CrappyAINodes.h"

#include "NeuronNetwork.h"
#include "Neuron.h"

#include <pugixml.hpp>

#include <cassert>
#include <cmath>
#include <iostream>

using namespace pugi;

// ####################################
// # NodeValue #
// ####################################
namespace cria
{
	int int_null_value_ = 0;
	float float_null_value_ = 0.0f;

	void test_xml_save()
	{
		AddNode addAINode(NodeValue(NodeValue::INT_VALUE, 10), NodeValue(NodeValue::FLOAT_VALUE, 12.5f));
		xml_document xmlDoc;
		xml_node node = xmlDoc.append_child(CRIA_XML_ELEMENT_AINODE);
		addAINode.saveToXML(node);

		xmlDoc.save_file("save_test.xml");
		
	}

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
			assert(false);
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

	void NodeValue::saveToXML(xml_node& node, const char* name) const
	{
		node.append_attribute(CRIA_XML_ATTRIBUTE_NAME).set_value(name);
		node.append_attribute(CRIA_XML_ATTRIBUTE_TYPE).set_value(m_Type);

		if (m_Type == INT_VALUE)
			node.set_value(std::to_string(m_IntValue).c_str());
		else if (m_Type == FLOAT_VALUE)
			node.set_value(std::to_string(m_FloatValue).c_str());
		else if (isIndex())
			node.set_value(std::to_string(m_IndexValue).c_str());
		else
			node.set_value("0");
	}
	void NodeValue::loadXML(const xml_node& node)
	{
		m_Type = node.attribute(CRIA_XML_ATTRIBUTE_TYPE).as_int(0);
		const char* valueStr = node.value();
		
		if (m_Type == INT_VALUE)
			m_IntValue = atoi(valueStr);
		else if (m_Type == FLOAT_VALUE)
			m_FloatValue = (float)atof(valueStr);
		else if (isIndex())
			m_IndexValue = atoi(valueStr);
		else
			m_IntValue = 0;
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
	void Node::append(Node* node)
	{
		Node* n = this;
		while (n->m_Next)
		{
			n = n->m_Next;
		}

		n->m_Next = node;
	}


	void Node::SaveAINodeStack(xml_node& node, Node* aiNode, const char* name)
	{
		node.set_name(CRIA_XML_ELEMENT_AINODESTACK);
		if (strlen(name))
			node.append_attribute(CRIA_XML_ATTRIBUTE_NAME).set_value(name);

		xml_node n;
		while (aiNode)
		{
			n = node.append_child(CRIA_XML_ELEMENT_AINODE);
			aiNode->saveToXML(n);

			aiNode = aiNode->m_Next;
		}

	}
	Node* Node::LoadAINodeStack(const xml_node& node)
	{
		assert(strcmp(node.name(), CRIA_XML_ELEMENT_AINODESTACK));

		if (strcmp(node.name(), CRIA_XML_ELEMENT_AINODESTACK))
		{
			Node* aiNodes = nullptr;

			for (xml_node n : node)
			{
				if (strcmp(n.name(), CRIA_XML_ELEMENT_AINODE))
				{
					if (aiNodes)
						aiNodes->append(LoadAINode(n));
					else
						aiNodes = LoadAINode(n);
				}
			}

			return aiNodes;
		} 
		if (strcmp(node.name(), CRIA_XML_ELEMENT_AINODE))
		{
			return LoadAINode(node);
		}
		
		return nullptr;
	}

	Node* Node::LoadAINode(const pugi::xml_node& xmlNode)
	{
		assert(strcmp(xmlNode.name(), CRIA_XML_ELEMENT_AINODE));

		CRIA_NODE_TYPE type = (CRIA_NODE_TYPE)xmlNode.attribute(CRIA_XML_ATTRIBUTE_TYPE).as_int(0);
		if (type == 0)
			return nullptr;

		switch (type)
		{
		case CRIA_NODE_ADD: 
			return new AddNode(xmlNode);
		case CRIA_NODE_SUBTRACT: 
			return new SubtractNode(xmlNode);
		case CRIA_NODE_MULTIPLY: 
			return new MultiplyNode(xmlNode);
		case CRIA_NODE_DIVIDE: 
			return new DivideNode(xmlNode);

		case CRIA_NODE_SET_ACTIVE_SLOT: 
			return new SetActiveSlotNode(xmlNode);
		
		case CRIA_NODE_IF_EQUAL: 
			return new IfEqualNode(xmlNode);
		case CRIA_NODE_IF_LESS: 
			return new IfLessNode(xmlNode);
		case CRIA_NODE_BREAK_IF_EQUAL: 
			return new BreakIfEqualNode(xmlNode);
		case CRIA_NODE_BREAK_IF_LESS:
			return new BreakIfLessNode(xmlNode);
		
		case CRIA_NODE_FOR:
			return new ForNode(xmlNode);

		case CRIA_NODE_GET_PIXEL: 
			return new GetPixelNode(xmlNode);
		default: 
			return nullptr;
		}
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

	TwoValueNode::TwoValueNode(const xml_node& node)
		: m_Var1(NodeValue::INT_VALUE, 0), 
		m_Var2(NodeValue::INT_VALUE, 2)
	{
		for (xml_node n : node) {
			if (strcmp(n.name(), CRIA_XML_ELEMENT_AINODEVALUE)) {
				const char* name = n.attribute(CRIA_XML_ATTRIBUTE_NAME).as_string();

				if (strcmp(name, "Var1"))
					m_Var1.loadXML(n);
				else if (strcmp(name, "Var2"))
					m_Var2.loadXML(n);
			}
		}
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

	void TwoValueNode::saveToXML(pugi::xml_node& node)
	{
		node.set_name(CRIA_XML_ELEMENT_AINODE);
		node.append_attribute(CRIA_XML_ATTRIBUTE_TYPE).set_value(std::to_string(getType()).c_str());

		xml_node varNode = node.append_child(CRIA_XML_ELEMENT_AINODEVALUE);
		m_Var1.saveToXML(varNode, "Var1");
		varNode = node.append_child(CRIA_XML_ELEMENT_AINODEVALUE);
		m_Var2.saveToXML(varNode, "Var2");
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

	BaseIfNode::BaseIfNode(const pugi::xml_node& node)
		: TwoValueNode(node),
		m_ActionOnTrue(nullptr),
		m_ActionOnFalse(nullptr)
	{
		for (xml_node n : node) {
			if (strcmp(n.name(), CRIA_XML_ELEMENT_AINODESTACK)) {
				const char* name = n.attribute(CRIA_XML_ATTRIBUTE_NAME).as_string();

				if (strcmp(name, "TrueNodes"))
					m_ActionOnTrue = LoadAINodeStack(n);
				else if (strcmp(name, "FalseNodes"))
					m_ActionOnFalse = LoadAINodeStack(n);
			}
		}
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

	void BaseIfNode::saveToXML(xml_node& node)
	{
		TwoValueNode::saveToXML(node);

		node.set_name(CRIA_XML_ELEMENT_AINODE);
		node.append_attribute(CRIA_XML_ATTRIBUTE_TYPE).set_value(std::to_string(getType()).c_str());

		xml_node n;
		if (m_ActionOnTrue)
		{
			n = node.append_child();
			SaveAINodeStack(n, m_ActionOnTrue, "TrueNodes");
		}
		if (m_ActionOnFalse) {
			n = node.append_child();
			SaveAINodeStack(n, m_ActionOnFalse, "FalseNodes");
		}
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

	SetActiveSlotNode::SetActiveSlotNode(const pugi::xml_node& node)
		: m_Slot(NodeValue::INT_VALUE, 0)
	{
		for (xml_node n : node) {
			if (strcmp(n.name(), CRIA_XML_ELEMENT_AINODEVALUE)) {
				const char* name = n.attribute(CRIA_XML_ATTRIBUTE_NAME).as_string();

				if (strcmp(name, "Slot"))
				{
					m_Slot.loadXML(n);
					break;
				}
			}
		}
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

	void SetActiveSlotNode::saveToXML(xml_node& node)
	{
		node.set_name(CRIA_XML_ELEMENT_AINODE);
		node.append_attribute(CRIA_XML_ATTRIBUTE_TYPE).set_value(std::to_string(getType()).c_str());

		xml_node n = node.append_child();
		m_Slot.saveToXML(n, "Slot");
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

	ForNode::ForNode(const pugi::xml_node& node)
		: m_Start(NodeValue::INT_VALUE, 0),
		m_Limit(NodeValue::INT_VALUE, 0),
		m_Incrementer(NodeValue::INT_VALUE, 0),
		m_IndexWriteLoc(NodeValue::INT_VALUE, 0),
		m_ActionNodes(nullptr)
	{
		for (xml_node n : node) {
			if (strcmp(n.name(), CRIA_XML_ELEMENT_AINODEVALUE)) {
				const char* name = n.attribute(CRIA_XML_ATTRIBUTE_NAME).as_string();

				if (strcmp(name, "Start"))
					m_Start.loadXML(n);
				else if (strcmp(name, "Limit"))
					m_Limit.loadXML(n);
				else if (strcmp(name, "Incrementer"))
					m_Incrementer.loadXML(n);
				else if (strcmp(name, "IndexWriteLoc"))
					m_IndexWriteLoc.loadXML(n);

			} else if (strcmp(n.name(), CRIA_XML_ELEMENT_AINODESTACK) && !m_ActionNodes)
			{
				m_ActionNodes = LoadAINodeStack(n);

			}
		}
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

	void ForNode::saveToXML(pugi::xml_node& node)
	{
		node.set_name(CRIA_XML_ELEMENT_AINODE);
		node.append_attribute(CRIA_XML_ATTRIBUTE_TYPE).set_value(std::to_string(getType()).c_str());

		xml_node varNode = node.append_child();
		m_Start.saveToXML(varNode, "Start");
		varNode = node.append_child();
		m_Limit.saveToXML(varNode, "Limit");
		varNode = node.append_child();
		m_Incrementer.saveToXML(varNode, "Incrementer");
		varNode = node.append_child();
		m_IndexWriteLoc.saveToXML(varNode, "IndexWriteLoc");

		xml_node stackNode = node.append_child();
		SaveAINodeStack(stackNode, m_ActionNodes);
	}
}

// ##############################################
// # GetPixelNode #
// ##############################################
namespace cria {
	GetPixelNode::GetPixelNode(NodeValue testX, NodeValue testY)
		: TwoValueNode(testX, testY)
	{
	}

	node_return GetPixelNode::beCrappyInt(int x, int y, Neuron* neuron)
	{
		NeuronNetwork* network = neuron->getNetwork();

		if (x < 0 || x >= network->getBitmapWidth() || y < 0 || y >= network->getBitmapHeight())
			return 0;

		int p = network->getBitmap()[x + y * network->getBitmapWidth()];
		neuron->getActiveData()->reset(NodeValue::INT_VALUE, p);
		return p;
	}
	node_return GetPixelNode::beCrappyFloat(float a, float b, Neuron* neuron)
	{
		return beCrappyInt((int)a, (int)b, neuron);
	}
}
