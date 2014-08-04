#pragma once
#include <DerpLex/ASTBase.h>
#include <DerpLex/ASTExpr.h>

#pragma warning(disable: 4505)
/*
                            Node
                             |
                ----------------------
                |                    |
       BinaryStatementNode       NamedNode
                |                    |
           CallStatementNode     -----------------------------
                            |                           |
                    NamedCompoundNode               DirectiveNode
                     |             |
               CallableNode   RunnableNode
                                   |
                               DataNode
*/
namespace Derp
{
    namespace AST
    {

        /////////////////////////////////////////////////////////////////////////////////
        //
        //  Statement nodes
        //
        /////////////////////////////////////////////////////////////////////////////////

        template<class NodeIdType>  //  Node
        struct BinaryStatementNode : Node<NodeIdType>
        {
            typedef typename Node::nodeid_type nodeid_type;
            typedef typename Node::value_type value_type;
            typedef typename Node::string_type string_type;
            typedef typename Node::context_type context_type;
            typedef ExpressionNode<NodeIdType> expr_type;
            typedef std::shared_ptr<expr_type> exprptr_type;
            typedef typename expr_type::result_type result_type;

            BinaryStatementNode() = delete;
            BinaryStatementNode(const BinaryStatementNode&) = delete;

            BinaryStatementNode(
                const nodeid_type typeId,
                const string_type& opcode,
                const exprptr_type& lvalue,
                const exprptr_type rvalue)
                : Node(typeId, opcode), lvalue_(lvalue), rvalue_(rvalue)
            {}


            BinaryStatementNode& operator=(const BinaryStatementNode&) = delete;
            BinaryStatementNode& operator==(const BinaryStatementNode&) = delete;


            virtual result_type evaluateLeft(context_type& context) const
            {
                if (!lvalue_)
                {
                    throw std::logic_error("[BinaryStatementNode] lvalue cannot be nullptr");
                }

                return lvalue_->evaluate(context);
            }

            virtual result_type evaluateRight(context_type& context) const
            {
                if (!rvalue_)
                {
                    throw std::logic_error("[BinaryStatementNode] rvalue cannot be nullptr");
                }

                return rvalue_->evaluate(context);
            }
            

            virtual value_type execute(context_type& context, XMLNode& root) const override;


        protected:

            virtual void internal_SerializeOut(XMLNode& node) const override;
            virtual value_type internal_Execute(context_type& context, XMLNode& node) const override;


            const exprptr_type  lvalue_;
            const exprptr_type  rvalue_;
        };


        template<class NodeIdType>  //  BinaryStatementNode
        struct CallStatementNode : BinaryStatementNode<NodeIdType>
        {
            typedef typename BinaryStatementNode::exprptr_type exprptr_type;

            CallStatementNode() = delete;
            CallStatementNode(const CallStatementNode&) = delete;

            CallStatementNode(
                const nodeid_type typeId,
                const string_type& opcode,
                const exprptr_type& functionName,
                const std::shared_ptr<ArgumentsExpressionNode<NodeIdType>>  argument)
                : BinaryStatementNode(typeId, opcode, functionName, argument)
            {}


            CallStatementNode& operator=(const CallStatementNode&) = delete;
            CallStatementNode& operator==(const CallStatementNode&) = delete;


            virtual value_type execute(context_type& context, XMLNode& node) const override;


        protected:

            virtual value_type internal_ExecuteCall(
                context_type& context,
                XMLNode& node,
                Node& callableNode,
                value_type& newArguments) const;

            virtual std::shared_ptr<Node<NodeIdType>> resolveCallableObject(
                context_type& context,
                const string_type& objectName) const
            {
                //  Part 3 - same as CallStatementNode::internal_Execute
                auto callableObject = context.findFunctionObject(objectName);
                if (callableObject == nullptr)
                {
                    context.ReportError(L"Error: Unresolved reference to function `" + objectName + L"`");
                }

                return callableObject;
            }

        };




        /////////////////////////////////////////////////////////////////////////////////
        //
        //  Primary/Outer nodes
        //
        /////////////////////////////////////////////////////////////////////////////////

        template<class NodeIdType>  //  Node
        struct NamedNode : Node<NodeIdType>
        {
            NamedNode() = delete;
            NamedNode(const NamedNode&) = delete;

            explicit NamedNode(
                const nodeid_type typeId,
                const string_type& opcode,
                const string_type& name)
                : Node(typeId, opcode), name_(name)
            {
            }


            virtual const string_type& getName() const
            {
                return name_;
            }


        protected:

            virtual void internal_SerializeOut(XMLNode& node) const override;
            virtual value_type internal_Execute(context_type& context, XMLNode& node) const override;


        private:

            const string_type name_;
        };


        template<class NodeIdType>  //  NamedNode
        struct DirectiveNode : NamedNode<NodeIdType>
        {
            DirectiveNode() = delete;
            DirectiveNode(const DirectiveNode&) = delete;
            explicit DirectiveNode(
                const nodeid_type typeId,
                const string_type& opcode,
                const string_type& name)
                : NamedNode(typeId, opcode, name)
            {}


            DirectiveNode& operator=(const DirectiveNode&) = delete;
            DirectiveNode& operator==(const DirectiveNode&) = delete;
        };


        template<class NodeIdType>  //  NamedNode
        struct NamedCompoundNode : NamedNode<NodeIdType>
        {
            typedef BinaryStatementNode<NodeIdType> statementnode_type;

            NamedCompoundNode() = delete;
            NamedCompoundNode(const NamedCompoundNode&) = delete;

            explicit NamedCompoundNode(
                const nodeid_type typeId,
                const string_type& opcode,
                const string_type& name)
                : NamedNode(typeId, opcode, name)
            {
            }


            virtual void addChild(std::shared_ptr<statementnode_type> node)
            {
                if (node == nullptr)
                {
                    throw std::invalid_argument("node cannot be nullptr");
                }

                children_.push_back(std::move(node));
            }


        protected:

            virtual void internal_SerializeOut(XMLNode& node) const override;
            virtual value_type internal_Execute(context_type& context, XMLNode& node) const override;

        private:

            std::vector<std::shared_ptr<statementnode_type>>  children_;
        };


        template<class NodeIdType>  //  NamedCompoundNode
        struct CallableNode : NamedCompoundNode<NodeIdType>
        {
            CallableNode() = delete;
            CallableNode(const CallableNode&) = delete;
            explicit CallableNode(
                const nodeid_type typeId,
                const string_type& opcode,
                const string_type& name,
                const value_type& parameters)
                : NamedCompoundNode(typeId, opcode, name), parameters_(parameters)
            {
                if (parameters.getType() != value_type::Tuple)
                {
                    throw std::invalid_argument("call arguments must be passed as a tuple");
                }
            }


            CallableNode& operator=(const CallableNode&) = delete;
            CallableNode& operator==(const CallableNode&) = delete;

            virtual bool hasParameters() const
            {
                return parameters_.getTupleSize() != 0;
            }


            virtual value_type execute(context_type& context, XMLNode& root) const override;


        protected:

            virtual void internal_SerializeOut(XMLNode& node) const override;


        private:

            const value_type    parameters_;
        };


        template<class NodeIdType>  //  CallableNode
        struct DataNode : CallableNode<NodeIdType>
        {
            DataNode() = delete;
            DataNode(const DataNode&) = delete;
            explicit DataNode(
                const nodeid_type typeId,
                const string_type& opcode,
                const string_type& name)
                : CallableNode(typeId, opcode, name, value_type(value_type::tuple_type()))
            {}

            DataNode& operator=(const DataNode&) = delete;
            DataNode& operator==(const DataNode&) = delete;
        };

    }


    namespace AST
    {
        template<class NodeIdType>
        void Node<NodeIdType>::serializeOut(XMLNode& root) const
        {
            XMLNode node(opcode_);
            internal_SerializeOut(node);
            root.addNode(node);
        }

 
        template<class NodeIdType>
        typename Node<NodeIdType>::value_type Node<NodeIdType>::execute(context_type& context, XMLNode& root) const
        {
            return internal_Execute(context, root);
        }


        template<class NodeIdType>
        void Node<NodeIdType>::internal_SerializeOut(XMLNode& /*node*/) const
        {}


        template<class NodeIdType>
        typename Node<NodeIdType>::value_type Node<NodeIdType>::internal_Execute(context_type& /*context*/, XMLNode& /*root*/) const
        {
            return value_type();
        }




        template<class NodeIdType>  //  Node
        typename BinaryStatementNode<NodeIdType>::value_type
            BinaryStatementNode<NodeIdType>::execute(context_type& context, XMLNode& root) const
        {
            XMLNode node(getOpcode());
            auto result = Node::execute(context, node);
            root.addNode(node);
            return result;
        }


        template<class NodeIdType>  //  Node
        void BinaryStatementNode<NodeIdType>::internal_SerializeOut(XMLNode& node) const
        {
            Node::internal_SerializeOut(node);
            if (lvalue_)
            {
                lvalue_->serializeOut(node);
            }

            if (rvalue_)
            {
                rvalue_->serializeOut(node);
            }
        }


        template<class NodeIdType>  //  Node
        typename BinaryStatementNode<NodeIdType>::value_type
            BinaryStatementNode<NodeIdType>::internal_Execute(context_type& context, XMLNode& node) const
        {
            auto result = Node::internal_Execute(context, node);
            node.setAttribute(L"action", lvalue_->evaluate(context).to_string());
            if (rvalue_)
            {
                node.setAttribute(L"data", rvalue_->evaluate(context).to_string());
            }

            return result;
        }




        template<class NodeIdType>  //  BinaryStatementNode
        typename CallStatementNode<NodeIdType>::value_type CallStatementNode<NodeIdType>::execute(context_type& context, XMLNode& node) const
        {
            auto objectNameValue = lvalue_->evaluate(context).copyValue();
            if (objectNameValue.isValid() == false)
            {
                return value_type();
            }

            if (objectNameValue.getType() != value_type::Identifier)
            {
                context.ReportError("[CallStatementNode] lvalue must be an identifier.");
                return value_type();
            }

            auto callablObject = resolveCallableObject(context, objectNameValue.getIdentifier());
            if (callablObject == nullptr)
            {
                return value_type();
            }

            auto arguments(evaluateRight(context).copyValue());
            if (arguments.isValid())
            {
                return internal_ExecuteCall(context, node, *callablObject, arguments);
            }

            return value_type();
        }






        template<class NodeIdType>  //  Node
        void NamedNode<NodeIdType>::internal_SerializeOut(XMLNode& node) const
        {
            Node::internal_SerializeOut(node);
            node.setAttribute(L"name", getName());
        }


        template<class NodeIdType>  //  Node
        typename NamedNode<NodeIdType>::value_type NamedNode<NodeIdType>::internal_Execute(context_type& context, XMLNode& node) const
        {
            auto result = Node::internal_Execute(context, node);
            node.setAttribute(L"name", getName());

            return result;
        }




        template<class NodeIdType>  //  NamedNode
        void NamedCompoundNode<NodeIdType>::internal_SerializeOut(XMLNode& node) const
        {
            NamedNode::internal_SerializeOut(node);
            for (auto& child : children_)
            {
                child->serializeOut(node);
            }
        }


        template<class NodeIdType>  //  NamedNode
        typename NamedCompoundNode<NodeIdType>::value_type NamedCompoundNode<NodeIdType>::internal_Execute(context_type& context, XMLNode& node) const
        {
            NamedNode::internal_Execute(context, node);
            for (auto& child : children_)
            {
                child->execute(context, node);
            }

            return value_type();
        }




        template<class NodeIdType>  //  NamedCompoundNode
        typename CallableNode<NodeIdType>::value_type CallableNode<NodeIdType>::execute(context_type& context, XMLNode& root) const
        {
            if (context.isExcluded(getName()))
            {
                throw std::domain_error("Trying to generate code on an excluded object.");
            }

            if (context.getLocalVariableCount() != 0)
            {
                throw std::domain_error("Callable object must be entered with no local variables in the context.");
            }

            auto arguments = context.getArguments();

            if (arguments.isValid() == false && parameters_.getTupleSize() != 0)
            {
                context.ReportError("OOPS! Arguments are required but set to `Invalid`.");
                return value_type();
            }
            else if (arguments.isValid() && arguments.getTupleSize() != parameters_.getTupleSize())
            {
                context.ReportError("OOPS! Problem in pipeline. Number of arguments does not match number of parameters");
                return value_type();
            }

            for (auto i = 0U; i < parameters_.getTupleSize(); ++i)
            {
                const auto& arg = arguments.getTupleData()[i];
                switch (arg.getType())
                {
                case value_type::typeid_type::Identifier:
                case value_type::typeid_type::Boolean:
                case value_type::typeid_type::Integer:
                case value_type::typeid_type::String:
                case value_type::typeid_type::Tuple:
                    context.setLocalVariable(parameters_.getTupleData()[i].to_string(), arg);
                    break;

                default:
                    throw std::domain_error("Unsupported argument type. Must be a tuple, boolean, string, integer, or identifier.");
                }
            }

            return NamedCompoundNode::execute(context, root);
        }


        template<class NodeIdType>  //  NamedCompoundNode
        void CallableNode<NodeIdType>::internal_SerializeOut(XMLNode& node) const
        {
            if (parameters_.getTupleSize())
            {
                XMLNode parametersNode(L"Parameters");
                for (auto&& param : parameters_)
                {
                    XMLNode parameterNode(L"Parameter");
                    parameterNode.setAttribute(L"name", param.getVariable());
                    parametersNode.addNode(parameterNode);
                }
                node.insertNode(parametersNode);
            }
            NamedCompoundNode::internal_SerializeOut(node);
        }

    }
}
