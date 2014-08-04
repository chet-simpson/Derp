#pragma once
#include <cstdint>
#include <string>
#include <vector>


namespace Derp
{

    struct Value
    {
        enum Type
        {
            Invalid,
            Identifier,     //  Symbolic literal
            Variable,
            Tuple,
            Boolean,        //  Literal
            Integer,        //  Literal
            String,         //  Literal
        };

        typedef Type typeid_type;
        typedef std::wstring string_type;
        typedef std::int64_t integer_type;
        typedef std::vector<Value> tuple_type;

        Value();
        Value(typeid_type typeId, string_type text);
        Value(const bool& boolean, nullptr_t);
        explicit Value(integer_type integer);
        explicit Value(string_type text);
        explicit Value(tuple_type data);


        bool isValid() const { return typeId != Invalid; }
        typeid_type getType() const { return typeId; }

        string_type to_string() const;
        const string_type& getTextValue() const;
        const string_type& getString() const;
        const string_type& getIdentifier() const;
        const string_type& getVariable() const;
        integer_type getInteger() const;

        const tuple_type::size_type getTupleSize() const;
        const tuple_type& getTupleData() const;
        tuple_type::const_iterator begin() const;
        tuple_type::const_iterator end() const;
        tuple_type::iterator begin();
        tuple_type::iterator end();


    private:

        typeid_type     typeId;
        string_type     text;
        integer_type    integer;
        bool            boolean;
        tuple_type      tuple_data;
    };

}

