#pragma once

#include "Crappy.h"

#define CRIA_NODE_FOR_LOOP_MAX_COUNT            1000

#define CRIA_XML_ELEMENT_AINODESTACK            "AINodeStack"
#define CRIA_XML_ELEMENT_AINODE                 "AINode"
#define CRIA_XML_ATTRIBUTE_TYPE                 "type"

#define CRIA_XML_ELEMENT_AINODEVALUE            "AINodeValue"
#define CRIA_XML_ATTRIBUTE_NAME                 "name"

namespace pugi
{
	class xml_node;
}

namespace cria {

	class Neuron;
	typedef int node_return;

	typedef enum CRAPPY_API CRIA_NODE_TYPE_ {
		CRIA_NODE_ADD = 1,
		CRIA_NODE_SUBTRACT,
		CRIA_NODE_MULTIPLY,
		CRIA_NODE_DIVIDE,

		CRIA_NODE_SET_ACTIVE_SLOT,

		CRIA_NODE_IF_EQUAL,
		CRIA_NODE_IF_LESS,
		CRIA_NODE_BREAK_IF_EQUAL,
		CRIA_NODE_BREAK_IF_LESS,

		CRIA_NODE_FOR,

		CRIA_NODE_GET_PIXEL
	} CRIA_NODE_TYPE;



	CRAPPY_API void test_xml_save();



	class CRAPPY_API NodeValue
	{
	private:
		friend class Neuron;
	public:
		typedef unsigned int ValueType;
		
		static const ValueType INT_VALUE = 0;
		static const ValueType FLOAT_VALUE = 1;
		static const ValueType INDEX_INT_VALUE = 2;
		static const ValueType INDEX_FLOAT_VALUE = 3;
		
	protected:
		union
		{
			int m_IntValue;
			float m_FloatValue;
			int m_IndexValue;
		};

		ValueType m_Type;

	public:
		NodeValue();
		NodeValue(ValueType type, int value);
		NodeValue(ValueType type, float value);

		void reset(ValueType type, int value);
		void reset(ValueType type, float value);

		bool isInt() const;
		bool isFloat() const;
		bool isIndex() const;
		ValueType getType() const;

		int& getIntValue(Neuron* neuron);
		float& getFloatValue(Neuron* neuron);
		
		float getAsFloat(Neuron* neuron);
		int getAsInt(Neuron* neuron);

		void set(int i, Neuron* neuron);
		void set(float f, Neuron* neuron);

		void saveToXML(pugi::xml_node& node, const char* name) const;
		void loadXML(const pugi::xml_node& node);
	};



	// ##############################################
	// # base Nodes #
	// ##############################################
	class CRAPPY_API Node
	{
	public:
		Node* m_Next;
	protected:
		Node() : m_Next(nullptr) {}
	public:
		virtual ~Node() {}

		node_return callStack(Neuron* neuron);
		void append(Node* node);

		virtual node_return beCrappy(Neuron* neuron) = 0;

		virtual CRIA_NODE_TYPE getType() = 0;

		virtual void saveToXML(pugi::xml_node& node) {};

		static void SaveAINodeStack(pugi::xml_node& node, Node* aiNodes, const char* name = "");
		static Node* LoadAINodeStack(const pugi::xml_node& node);
		static Node* LoadAINode(const pugi::xml_node& node);
	};
	class CRAPPY_API TwoValueNode : public Node {
	protected:
		NodeValue m_Var1;
		NodeValue m_Var2;
		
		TwoValueNode(const NodeValue& var1, const NodeValue& var2);
		TwoValueNode(const pugi::xml_node& node);
	public:
		node_return beCrappy(Neuron* neuron) override;

		virtual void saveToXML(pugi::xml_node& node) override;
	protected:
		virtual node_return beCrappyInt(int a, int b, Neuron* neuron) = 0;
		virtual node_return beCrappyFloat(float a, float b, Neuron* neuron) = 0;
	};
	class CRAPPY_API BaseIfNode : public TwoValueNode
	{
		
	protected:
		Node* m_ActionOnTrue;
		Node* m_ActionOnFalse;
		
		BaseIfNode(const NodeValue& var1, const NodeValue& var2, Node* trueAction, Node* falseAction);

		node_return activateTrueNode(Neuron* neuron);
		node_return activateFalseNode(Neuron* neuron);
	public:
		BaseIfNode(const pugi::xml_node& node);
		void saveToXML(pugi::xml_node& node) override;
	};



	// ##############################################
	// # AddNode #
	// ##############################################
	class CRAPPY_API AddNode : public TwoValueNode
	{
	public:
		AddNode(const NodeValue& var1, const NodeValue& var2);
		AddNode(const pugi::xml_node& node) : TwoValueNode(node) {}

	protected:
		node_return beCrappyInt(int a, int b, Neuron* neuron) override;
		node_return beCrappyFloat(float a, float b, Neuron* neuron) override;

	public:
		virtual CRIA_NODE_TYPE getType() override {return CRIA_NODE_ADD;};
	};
	// ##############################################
	// # SubtractNode #
	// ##############################################
	class CRAPPY_API SubtractNode : public TwoValueNode {
	public:
		SubtractNode(const NodeValue& var1, const NodeValue& var2);
		SubtractNode(const pugi::xml_node& node) : TwoValueNode(node) {}

	protected:
		node_return beCrappyInt(int a, int b, Neuron* neuron) override;
		node_return beCrappyFloat(float a, float b, Neuron* neuron) override;

	public:
		virtual CRIA_NODE_TYPE getType() override { return CRIA_NODE_SUBTRACT; };
	};
	// ##############################################
	// # MultiplyNode #
	// ##############################################
	class CRAPPY_API MultiplyNode : public TwoValueNode {
	public:
		MultiplyNode(const NodeValue& var1, const NodeValue& var2);
		MultiplyNode(const pugi::xml_node& node) : TwoValueNode(node) {}

	protected:
		node_return beCrappyInt(int a, int b, Neuron* neuron) override;
		node_return beCrappyFloat(float a, float b, Neuron* neuron) override;

	public:
		virtual CRIA_NODE_TYPE getType() override { return CRIA_NODE_MULTIPLY; };
	};
	// ##############################################
	// # DivideNode #
	// ##############################################
	class CRAPPY_API DivideNode : public TwoValueNode {
	public:
		DivideNode(const NodeValue& var1, const NodeValue& var2);
		DivideNode(const pugi::xml_node& node) : TwoValueNode(node) {}

	protected:
		node_return beCrappyInt(int a, int b, Neuron* neuron) override;
		node_return beCrappyFloat(float a, float b, Neuron* neuron) override;

	public:
		virtual CRIA_NODE_TYPE getType() override { return CRIA_NODE_DIVIDE; };
	};



	// ##############################################
	// # SetActiveSlotNode #
	// ##############################################
	class CRAPPY_API SetActiveSlotNode : public Node
	{
	private:
		NodeValue m_Slot;
	public:
		SetActiveSlotNode(const NodeValue& slot);
		SetActiveSlotNode(const pugi::xml_node& node);

		virtual node_return beCrappy(Neuron* neuron) override;

		virtual CRIA_NODE_TYPE getType() override { return CRIA_NODE_SET_ACTIVE_SLOT; }

		void saveToXML(pugi::xml_node& node) override;
	};



	// ##############################################
	// # IfEqualNode #
	// ##############################################
	class CRAPPY_API IfEqualNode : public BaseIfNode
	{
	public:
		IfEqualNode(const NodeValue& var1, const NodeValue& var2, Node* trueAction, Node* falseAction);
		IfEqualNode(const pugi::xml_node& node) : BaseIfNode(node) {}
	protected:
		node_return beCrappyInt(int a, int b, Neuron* neuron) override;
		node_return beCrappyFloat(float a, float b, Neuron* neuron) override;
	public:
		CRIA_NODE_TYPE getType() override { return CRIA_NODE_IF_EQUAL; };
	};
	// ##############################################
	// # IfLessNode #
	// ##############################################
	class CRAPPY_API IfLessNode : public BaseIfNode {
	public:
		IfLessNode(const NodeValue& var1, const NodeValue& var2, Node* trueAction, Node* falseAction);
		IfLessNode(const pugi::xml_node& node) : BaseIfNode(node) {}
	protected:
		node_return beCrappyInt(int a, int b, Neuron* neuron) override;
		node_return beCrappyFloat(float a, float b, Neuron* neuron) override;
	public:
		CRIA_NODE_TYPE getType() override { return CRIA_NODE_IF_LESS; };
	};
	// ##############################################
	// # BreakIfEqualNode #
	// ##############################################
	class CRAPPY_API BreakIfEqualNode : public TwoValueNode {
	public:
		BreakIfEqualNode(const NodeValue& var1, const NodeValue& var2);
		BreakIfEqualNode(const pugi::xml_node& node) : TwoValueNode(node) {}
	protected:
		node_return beCrappyInt(int a, int b, Neuron* neuron) override;
		node_return beCrappyFloat(float a, float b, Neuron* neuron) override;
	public:
		CRIA_NODE_TYPE getType() override { return CRIA_NODE_BREAK_IF_EQUAL; };
	};
	// ##############################################
	// # BreakIfLessNode #
	// ##############################################
	class CRAPPY_API BreakIfLessNode : public TwoValueNode {
	public:
		BreakIfLessNode(const NodeValue& var1, const NodeValue& var2);
		BreakIfLessNode(const pugi::xml_node& node) : TwoValueNode(node) {}
	protected:
		node_return beCrappyInt(int a, int b, Neuron* neuron) override;
		node_return beCrappyFloat(float a, float b, Neuron* neuron) override;
	public:
		CRIA_NODE_TYPE getType() override { return CRIA_NODE_BREAK_IF_LESS; };
	};



	// ##############################################
	// # ForNode #
	// ##############################################
	class CRAPPY_API ForNode : public Node
	{
	private:
		NodeValue m_Start;
		NodeValue m_Limit;
		NodeValue m_Incrementer;
		NodeValue m_IndexWriteLoc;

		Node* m_ActionNodes;
	public:
		ForNode(const NodeValue& start, const NodeValue& limit, const NodeValue& incrementer, const NodeValue& indexWriteLoc, Node* actionNodes);
		ForNode(const pugi::xml_node& node);

		node_return beCrappy(Neuron* neuron) override;
		CRIA_NODE_TYPE getType() override { return CRIA_NODE_FOR; }

		void saveToXML(pugi::xml_node& node) override;;
	};



	// ##############################################
	// # GetPixelNode #
	// ##############################################
	class CRAPPY_API GetPixelNode : public TwoValueNode
	{

	public:
		GetPixelNode(NodeValue testX, NodeValue testY);
		GetPixelNode(const pugi::xml_node& node) : TwoValueNode(node) {}

		CRIA_NODE_TYPE getType() override { return CRIA_NODE_GET_PIXEL; }
	protected:
		node_return beCrappyInt(int a, int b, Neuron* neuron) override;
		node_return beCrappyFloat(float a, float b, Neuron* neuron) override;
	public:;
	};
}
