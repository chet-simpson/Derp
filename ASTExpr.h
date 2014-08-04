#pragma once
#include <DerpLex/ASTBase.h>
#pragma warning(disable: 4505)


namespace Derp
{
    namespace AST
    {
        struct ExpressionResult
        {
            typedef Value value_type;

            ExpressionResult() : value_(&shadow_) {}

            ExpressionResult(const ExpressionResult& other)
                : shadow_(other.shadow_), value_(other.isOwned() ? &shadow_ : other.value_)
            {}

            explicit ExpressionResult(value_type value)
                : shadow_(value), value_(&shadow_)
            {}

            explicit ExpressionResult(value_type* valuePtr)
                : value_(valuePtr != nullptr ? valuePtr : &shadow_)
            {
                if (valuePtr == nullptr)
                {
                    throw std::invalid_argument("valuePtr cannot be null");
                }
            }


            bool isOwned() const
            {
                return value_ == &shadow_;
            }

            //bool isValid() const
            //{
            //    return value_->isValid();
            //}

            std::wstring to_string() const
            {
                return value_->to_string();
            }

            const value_type copyValue() const
            {
                return *value_;
            }


            operator const value_type&() const
            {
                return *value_;
            }

            ExpressionResult& operator=(const ExpressionResult& other)
            {
                *value_ = *other.value_;
                return *this;
            }

            ExpressionResult& operator=(const value_type& value)
            {
                *value_ = value;
                return *this;
            }


        private:

            value_type          shadow_;
            value_type* const   value_;
        };



        /////////////////////////////////////////////////////////////////////////////////
        //
        //  Expression nodes
        //
        /////////////////////////////////////////////////////////////////////////////////

        template<class NodeIdType>  //  Node
        struct ExpressionNode : Node<NodeIdType>
        {
            typedef typename Node::nodeid_type nodeid_type;
            typedef typename Node::value_type value_type;
            typedef typename Node::string_type string_type;
            typedef typename Node::context_type context_type;
            typedef ExpressionResult result_type;

            ExpressionNode() = delete;
            ExpressionNode(const ExpressionNode&) = delete;

            ExpressionNode(
                const nodeid_type typeId,
                const string_type& opcode)
                : Node(typeId, opcode)
            {}


            ExpressionNode& operator=(const ExpressionNode&) = delete;
            ExpressionNode& operator==(const ExpressionNode&) = delete;


            virtual result_type evaluate(context_type& context) const = 0;


        private:

            virtual value_type execute(context_type& /*context*/, XMLNode& /*node*/) const override
            {
                throw std::runtime_error("Cannot call execute() here");
            }
        };


        template<class NodeIdType>  //  ExpressionNode
        struct ValueExpressionNode : ExpressionNode<NodeIdType>
        {
            typedef typename ExpressionNode::nodeid_type nodeid_type;
            typedef typename ExpressionNode::value_type value_type;
            typedef typename ExpressionNode::string_type string_type;
            typedef typename ExpressionNode::context_type context_type;

            ValueExpressionNode() = delete;
            ValueExpressionNode(const ValueExpressionNode&) = delete;

            ValueExpressionNode(
                const nodeid_type typeId,
                const string_type& opcode,
                const value_type& value)
                : ExpressionNode(typeId, opcode), value_(value)
            {
                if (value_.isValid() == false)
                {
                    throw std::runtime_error("Value type is not valid");
                }
            }

            explicit ValueExpressionNode(const value_type& value)
                : ExpressionNode(nodeid_type::Value, L"Value"), value_(value)
            {
                if (value_.isValid() == false)
                {
                    throw std::runtime_error("Value type is not valid");
                }
            }

            
            ValueExpressionNode& operator=(const ValueExpressionNode&) = delete;
            ValueExpressionNode& operator==(const ValueExpressionNode&) = delete;


            virtual result_type evaluate(context_type& context) const override
            {
                auto result = context.Resolve(value_);
                if (result.isValid() == false)
                {
                    context.ReportError(L"Unable to resolve `" + value_.to_string() + L"`");
                }

                return ExpressionResult(result);
            }

            virtual void internal_SerializeOut(XMLNode& node) const override
            {
                node.setAttribute(L"type", std::to_wstring(value_.getType()));
                node.setAttribute(L"value", value_.to_string());
            }


        protected:

            value_type  value_;

        };


        template<class NodeIdType>  //  ValueExpressionNode
        struct LiteralExpressionNode : ValueExpressionNode<NodeIdType>
        {
            typedef typename ValueExpressionNode::value_type value_type;
            typedef typename ValueExpressionNode::context_type context_type;

            LiteralExpressionNode() = delete;
            LiteralExpressionNode(const LiteralExpressionNode&) = delete;

            explicit LiteralExpressionNode(const value_type& value)
                : ValueExpressionNode(NodeIdType::Value, L"Literal", value)
            {}

            LiteralExpressionNode& operator=(const LiteralExpressionNode&) = delete;
            LiteralExpressionNode& operator==(const LiteralExpressionNode&) = delete;

            virtual result_type evaluate(context_type& /*context*/) const override
            {
                return ExpressionResult(value_);
            }
        };



        template<class NodeIdType>  //  ExpressionNode
        struct UnaryExpressionNode : ExpressionNode<NodeIdType>
        {
            typedef typename ExpressionNode expr_type;
            typedef typename ExpressionNode::nodeid_type nodeid_type;
            typedef typename ExpressionNode::value_type value_type;
            typedef typename ExpressionNode::string_type string_type;
            typedef typename ExpressionNode::context_type context_type;

            UnaryExpressionNode() = delete;
            UnaryExpressionNode(const UnaryExpressionNode&) = delete;

            UnaryExpressionNode(
                const nodeid_type typeId,
                const string_type& opcode,
                const std::shared_ptr<expr_type> value)
                : ExpressionNode(typeId, opcode), value_(value)
            {}

            UnaryExpressionNode& operator=(const UnaryExpressionNode&) = delete;
            UnaryExpressionNode& operator==(const UnaryExpressionNode&) = delete;


            virtual result_type evaluate(context_type& context) const override = 0;


        protected:

            virtual const value_type& getValue() const
            {
                return value_;
            }

            virtual void internal_SerializeOut(XMLNode& node) const override
            {
                value_->serializeOut(node);
            }


        private:

            std::shared_ptr<expr_type>  value_;
        };


        template<class NodeIdType>  //  ExpressionNode
        struct BinaryExpressionNode : ExpressionNode<NodeIdType>
        {
            typedef typename ExpressionNode expr_type;
            typedef typename ExpressionNode::nodeid_type nodeid_type;
            typedef typename ExpressionNode::value_type value_type;
            typedef typename ExpressionNode::string_type string_type;
            typedef typename ExpressionNode::context_type context_type;

            BinaryExpressionNode() = delete;
            BinaryExpressionNode(const BinaryExpressionNode&) = delete;

            BinaryExpressionNode(
                const nodeid_type typeId,
                const string_type& opcode,
                const std::shared_ptr<expr_type> lvalue,
                const std::shared_ptr<expr_type> rvalue)
                : ExpressionNode(typeId, opcode), lvalue_(lvalue), rvalue_(rvalue)
            {}


            BinaryExpressionNode& operator=(const BinaryExpressionNode&) = delete;
            BinaryExpressionNode& operator==(const BinaryExpressionNode&) = delete;


            virtual result_type evaluate(context_type& context) const override = 0;

            virtual value_type evaluateLeft(context_type& context) const
            {
                return context.Resolve(lvalue_);
            }

            virtual result_type evaluateRight(context_type& context) const
            {
                return rvalue_->evaluate(context);
            }


        protected:

            virtual void internal_SerializeOut(XMLNode& node) const override
            {
                if (lvalue_)
                {
                    lvalue_->serializeOut(node);
                }

                if (rvalue_)
                {
                    rvalue_->serializeOut(node);
                }
            }

            std::shared_ptr<expr_type>  lvalue_;
            std::shared_ptr<expr_type>  rvalue_;

        };


        template<class NodeIdType>  //  ExpressionNode
        struct ArgumentsExpressionNode : ExpressionNode<NodeIdType>
        {
            typedef typename ExpressionNode::nodeid_type nodeid_type;
            typedef typename ExpressionNode::value_type value_type;
            typedef typename ExpressionNode::string_type string_type;
            typedef typename ExpressionNode::context_type context_type;


            ArgumentsExpressionNode() : ExpressionNode(nodeid_type::Arguments, L"Arguments")
            {}

            ArgumentsExpressionNode(const ArgumentsExpressionNode&) = delete;

            ArgumentsExpressionNode& operator=(const ArgumentsExpressionNode&) = delete;
            ArgumentsExpressionNode& operator==(const ArgumentsExpressionNode&) = delete;


            virtual void addArgument(std::shared_ptr<ExpressionNode> argument)
            {
                arguments_.push_back(argument);
            }

            virtual result_type evaluate(context_type& context) const override
            {
                std::vector<value_type> newArguments;
                for (auto&& argument : arguments_)
                {
                    newArguments.push_back(argument->evaluate(context));
                }

                return ExpressionResult(value_type(newArguments));
            }


        protected:
            
            virtual void internal_SerializeOut(XMLNode& node) const override
            {
                for (auto&& argument : arguments_)
                {
                    argument->serializeOut(node);
                }
            }


        private:

            std::vector<std::shared_ptr<ExpressionNode>> arguments_;
        };



    }
}

