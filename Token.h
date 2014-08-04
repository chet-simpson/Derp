#include "Value.h"


namespace Derp
{
    template<class TokenIdType, class ValueType = Value>
    struct TokenT
    {
        typedef TokenIdType     tokenid_type;
        typedef ValueType       value_type;
        typedef typename value_type::string_type string_type;

        TokenT()
            : tokenId(tokenid_type::Invalid), lineNumber(0)
        {}

        TokenT(
            tokenid_type tokenId,
            std::size_t lineNumber,
            string_type tokenText,
            const value_type& value = value_type())
            : tokenId(tokenId), lineNumber(lineNumber), tokenText(tokenText), value(value)
        {}


        bool isValid() const { return getId() != tokenid_type::Invalid; }
        bool hasValue() const { return value.isValid(); }

        tokenid_type getId() const { return tokenId; }
        std::size_t getLine() const { return lineNumber; }
        const string_type& getText() const { return tokenText; }
        const value_type& getValue() const { return value; };

        bool operator==(tokenid_type id) const { return getId() == id; }


    private:

        tokenid_type    tokenId;
        std::size_t     lineNumber;
        string_type     tokenText;
        value_type      value;
    };

}
