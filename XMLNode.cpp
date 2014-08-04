#include <DerpLex/XMLNode.h>




XMLNode::XMLNode(const string_type& tagName)
: tagName(tagName)
{}




std::wostream& XMLNode::indent(std::wostream& output, size_t depth)
{
    return (output << std::setw(depth) << std::left << "");
}




void XMLNode::setAttribute(const string_type& name, const string_type& value)
{
    attributes.insert(std::make_pair(name, value));
}


void XMLNode::deleteAttribute(const string_type& name)
{
    auto attribute = attributes.find(name);
    if (attribute != attributes.end())
    {
        attributes.erase(attribute);
    }
}




void XMLNode::insertNode(const XMLNode& node)
{
    chilren.push_front(node);

}

void XMLNode::addNode(const XMLNode& node)
{
    chilren.push_back(node);
}




XMLNode::string_type XMLNode::to_string(std::size_t depth) const
{
    std::wostringstream out;

    indent(out, depth) << L"<" << tagName;
    for (auto&& attribute : attributes)
    {
        out << L" " << attribute.first << "=\"" << attribute.second << "\"";
    }
    out << (chilren.size() ? L">\n" : L"/>\n");

    if (chilren.size())
    {
        for (auto&& child : chilren)
        {
            out << child.to_string(depth + 2);
        }

        indent(out, depth) << L"</" << tagName << L">\n";
    }

    return out.str();
}
