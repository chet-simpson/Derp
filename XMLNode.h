#pragma once
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <deque>


struct XMLNode
{
    typedef std::wstring string_type;

    XMLNode(const XMLNode&) = default;
    explicit XMLNode(const string_type& tagName);
    virtual ~XMLNode() {}

    virtual void setAttribute(const string_type& name, const string_type& value);
    virtual void deleteAttribute(const string_type& name);
    virtual void addNode(const XMLNode& node);
    virtual void insertNode(const XMLNode& node);

    virtual string_type to_string(std::size_t depth = 0) const;

    XMLNode& operator=(const XMLNode&) = delete;

private:

    static std::wostream& indent(std::wostream& output, size_t depth);

    string_type             tagName;
    std::unordered_map<string_type, string_type> attributes;
    std::list<XMLNode>    chilren;
};