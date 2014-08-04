#pragma once
#include <DerpLex/Derp.h>
#include <DerpLex/Value.h>
#include <memory>
#include <map>
#include <vector>

namespace Derp
{
    template<class NodeType>
    struct IScopeContext
    {
        typedef NodeType                            node_type;
        typedef typename node_type::value_type      value_type;
        typedef typename node_type::string_type     string_type;
        typedef std::map<string_type, value_type>   variables_type;
        typedef std::vector<string_type>            excludedobjects_type;


        IScopeContext() = default;
        IScopeContext(const IScopeContext&) = default;
        IScopeContext& operator=(const IScopeContext&) = delete;


        virtual bool isExcluded(const string_type& name) const = 0;

        virtual Derp::ErrorReporting& getReporting() const = 0;
        virtual const value_type& getArguments() const = 0;
        virtual const excludedobjects_type& getExcludedObjects() const = 0;

        virtual void excludeObject(const string_type& name) = 0;
        virtual void setLocalVariable(const string_type& name, const value_type& value) = 0;

        virtual std::size_t getLocalVariableCount() const = 0;
        virtual value_type* findLocalVariable(const string_type& name) const = 0;
        virtual const value_type* findGameVariable(const string_type& name) const = 0;
        virtual const std::shared_ptr<node_type> findCallableObject(const string_type& name) const = 0;
        virtual const std::shared_ptr<node_type> findFunctionObject(const string_type& name) const = 0;

        virtual value_type Resolve(const value_type& value) const = 0;

        template<class MessageType, class... Args>
        void ReportError(const MessageType& msg, const Args&... args) const;

        template<class MessageType, class... Args>
        void ReportWarning(const MessageType& msg, const Args&... args) const;

        template<class MessageType, class... Args>
        void ReportWarningIf(bool condition, const MessageType& msg, const Args&... args) const;
    };

    template<class NodeType>
    template<class MessageType, class... Args>
    void IScopeContext<NodeType>::ReportError(const MessageType& msg, const Args&... args) const
    {
        getReporting().ReportError(0, msg, args...);
    }

    template<class NodeType>
    template<class MessageType, class... Args>
    void IScopeContext<NodeType>::ReportWarning(const MessageType& msg, const Args&... args) const
    {
        getReporting().ReportWarning(msg, args...);
    }

    template<class NodeType>
    template<class MessageType, class... Args>
    void IScopeContext<NodeType>::ReportWarningIf(bool condition, const MessageType& msg, const Args&... args) const
    {
        if (condition)
        {
            ReportWarning(msg, args...);
        }
    }
}

