#include "Value.h"


using namespace Derp;


Value::Value()
: typeId(Invalid), integer(0), boolean(false)
{}


Value::Value(typeid_type typeId, string_type text)
: typeId(typeId), integer(0), boolean(false), text(text)
{}


Value::Value(const bool& boolean, nullptr_t)
: typeId(Integer), integer(0), boolean(boolean)
{}


Value::Value(integer_type integer)
: typeId(Integer), integer(integer), boolean(false)
{}


Value::Value(string_type text)
: typeId(String), integer(0), boolean(false), text(text)
{}


Value::Value(tuple_type data)
: typeId(Tuple), integer(0), boolean(false), tuple_data(data)
{}




Value::string_type Value::to_string() const
{
    string_type outString;

    switch (getType())
    {
    case Invalid:
        outString = L"<INVALID>";
        break;

    case Boolean:
        outString = boolean == true ? L"true" : L"false";
        break;

    case Integer:
    {
        auto x = std::to_string(integer);
        outString = string_type(x.begin(), x.end());
    }
        break;

    case String:
    case Identifier:
    case Variable:
        outString = text;
        break;

    case Tuple:
        for (auto it = tuple_data.begin(); it != tuple_data.end(); ++it)
        {
            if (it != tuple_data.begin())
            {
                outString += L", ";
            }

            outString += it->to_string();
        }
        break;
    }

    return outString;
}


const Value::string_type& Value::getTextValue() const
{
    if (getType() != String && getType() != Identifier && getType() != Variable)
    {
        throw std::runtime_error("Value is not a string literal, identifier, or variable");
    }

    return text;
}


const Value::string_type& Value::getString() const
{
    if (getType() != String)
    {
        throw std::runtime_error("Value is not a string");
    }
    return text;
}


const Value::string_type& Value::getIdentifier() const
{
    if (getType() != Identifier)
    {
        throw std::runtime_error("Value is not a identifier");
    }

    return text;
}


const Value::string_type& Value::getVariable() const
{
    if (getType() != Variable)
    {
        throw std::runtime_error("Value is not a variable");
    }

    return text;
}


Value::integer_type Value::getInteger() const
{
    if (getType() != Integer)
    {
        throw std::runtime_error("Value is not a variable");
    }

    return integer;
}




const Value::tuple_type::size_type Value::getTupleSize() const
{
    if (getType() != Tuple)
    {
        throw std::runtime_error("Value is not a tuple");
    }

    return tuple_data.size();
}


const Value::tuple_type& Value::getTupleData() const
{
    if (getType() != Tuple)
    {
        throw std::runtime_error("Value is not a tuple");
    }

    return tuple_data;
}


Value::tuple_type::const_iterator Value::begin() const
{
    if (getType() != Tuple)
    {
        throw std::runtime_error("Value is not a tuple");
    }

    return tuple_data.cbegin();
}


Value::tuple_type::const_iterator Value::end() const
{
    if (getType() != Tuple)
    {
        throw std::runtime_error("Value is not a tuple");
    }

    return tuple_data.cend();
}


Value::tuple_type::iterator Value::begin()
{
    if (getType() != Tuple)
    {
        throw std::runtime_error("Value is not a tuple");
    }

    return tuple_data.begin();
}


Value::tuple_type::iterator Value::end()
{
    if (getType() != Tuple)
    {
        throw std::runtime_error("Value is not a tuple");
    }

    return tuple_data.end();
}

