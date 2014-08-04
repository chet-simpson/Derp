#pragma once
#include <DerpLex/Derp.h>
#include <DerpLex/Value.h>
#include <DerpLex/ScopeContext.h>
#include <DerpLex/XMLNode.h>

namespace Derp
{
    namespace AST
    {

        template<class NodeIdType>
        struct Node
        {
            typedef NodeIdType nodeid_type;
            typedef Derp::Value value_type;
            typedef value_type::string_type string_type;
            typedef IScopeContext<Node> context_type;


            Node() = delete;
            Node(const Node&) = delete;
            Node(const nodeid_type typeId, const string_type& opcode)
                : type_(typeId), opcode_(opcode)
            {
                if (opcode_.size() == 0)
                {
                    throw std::invalid_argument("opcode cannot be empty.");
                }
            }

            virtual ~Node() {}

            Node& operator=(const Node&) = delete;
            Node& operator==(const Node&) = delete;


            virtual nodeid_type getTypeId() const
            {
                return type_;
            }

            virtual bool isType(nodeid_type type) const
            {
                return getTypeId() == type;
            }

            const string_type& getOpcode() const
            {
                return opcode_;
            }


            virtual void serializeOut(XMLNode& root) const;
            virtual value_type execute(context_type& context, XMLNode& root) const;


        protected:


            virtual void internal_SerializeOut(XMLNode& node) const;
            virtual value_type internal_Execute(context_type& context, XMLNode& root) const;


        private:

            const nodeid_type                   type_;
            const string_type                   opcode_;
        };

    }
}
