#pragma once


namespace Derp
{
    struct Value
    {
        enum Type
        {
            Invalid,
            Integer,
            Identifier,
            Variable,
            String
        };

        typedef Type typeid_type;
        typedef std::wstring string_type;
        typedef std::int64_t integer_type;

        explicit Value() : typeId(Invalid), integer(0) {}

        Value(typeid_type typeId, string_type text)
            : typeId(typeId), text(std::move(text)), integer(0)
        {}

        explicit Value(integer_type integer)
            : typeId(Integer), text(std::to_wstring(integer)), integer(integer)
        {}

        bool isValid() const { return typeId != Invalid; }

        typeid_type getType() const { return typeId; }
        const string_type& getText() const { return text; }
        const string_type& getIdentifier() const { return text; }
        const string_type& getVariable() const { return text; }
        integer_type getInteger() const { return integer; }


    private:

        typeid_type     typeId;
        string_type     text;
        integer_type    integer;
    };
}

