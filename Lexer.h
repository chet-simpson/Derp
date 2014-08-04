#pragma once
#include <DerpLex/Derp.h>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>


namespace Derp
{

    template<class TokenType>
    struct LexerT
    {
        typedef TokenType                           token_type;
        typedef typename token_type::tokenid_type   tokenid_type;
        typedef typename token_type::string_type    input_type;
        typedef typename token_type::string_type    string_type;
        typedef typename token_type::value_type     value_type;
        typedef typename token_type::string_type::const_iterator    const_iterator;
        typedef typename token_type::string_type::value_type        char_type;
        typedef std::vector<token_type>             output_type;


        LexerT(ErrorReporting& reportingContext) : reportingContext_(reportingContext), lineNumber_(1) { }
        virtual ~LexerT() {}

        LexerT& operator=(const LexerT&) = delete;


        virtual void Reset()
        {
            lineNumber_ = 1;
            reservedWords_.clear();
            operators_.clear();
            tokenNames_.clear();
        }


        virtual bool AddOperator(const string_type& text, tokenid_type tokenId)
        {
            if (tokenNames_.find(tokenId) != tokenNames_.end())
            {
                reportingContext_.ReportFatal(
                    "Operator token `",
                    static_cast<std::underlying_type<decltype(tokenId)>::type>(tokenId),
                    "' (", text, ") already exists");
                return false;
            }

            if (operators_.find(text) != operators_.end())
            {
                reportingContext_.ReportFatal("Operator `", text, "' already exists");
                return false;
            }

            tokenNames_.insert(std::make_pair(tokenId, text));
            operators_.insert(std::make_pair(text, tokenId));

            return false;
        }


        virtual bool AddKeyword(const string_type& text, tokenid_type tokenId, bool hidden = false)
        {
            if (tokenNames_.find(tokenId) != tokenNames_.end())
            {
                reportingContext_.ReportFatal(
                    "Reserved word token `",
                    static_cast<std::underlying_type<decltype(tokenId)>::type>(tokenId),
                    "' (", text, ") already exists");
                return false;
            }

            if (hidden == false && reservedWords_.find(text) != reservedWords_.end())
            {
                reportingContext_.ReportFatal("Reserved word `", text, "' already exists");
                return false;
            }

            tokenNames_.insert(std::make_pair(tokenId, text));
            if (hidden == false)
            {
                reservedWords_.insert(std::make_pair(text, tokenId));
            }

            return true;
        }


        virtual string_type getTokenName(tokenid_type tokenId) const
        {
            auto word = tokenNames_.find(tokenId);
            return word == tokenNames_.end() ? L"<UNKNOWN>" : word->second;
        }


        virtual output_type Tokenize(const input_type& input, size_t lineNumber = 1)
        {
            output_type tokens;

            if (lineNumber > 0)
            {
                lineNumber_ = lineNumber;
            }

            auto inputBegin = input.cbegin(), inputEnd = input.cend();
            while (inputBegin != inputEnd)
            {
                token_type token;

                auto tokenEnd = Tokenize(inputBegin, inputEnd, token);
                tokens.push_back(token);
                if (token.isValid() == false && inputBegin == tokenEnd && tokenEnd != inputEnd)
                {
                    std::advance(tokenEnd, 1);
                }

                inputBegin = tokenEnd;
            }

            return tokens;
        }

        //  If inputBegin and inputEnd are the same inputBegin is returned and outputToken is set to invalid.
        virtual const_iterator Tokenize(
            const_iterator inputBegin,
            const_iterator inputEnd,
            token_type& outputToken)
        {
            outputToken = token_type(tokenid_type::Invalid, lineNumber_, token_type::string_type());

            //  Skip comments
            if (inputBegin != inputEnd && iscommentchar(*inputBegin))      //  Comment
            {
                while (inputBegin != inputEnd && isnewline(*inputBegin) == false)
                {
                    ++inputBegin;
                }
            }

            //  Reset token and stuff
            const char_type firstChar = inputBegin == inputEnd ? 0 : *inputBegin;
            const_iterator tokenEnd = inputBegin;

            if (inputBegin == inputEnd)
            {
                outputToken = token_type(tokenid_type::InputEnd, lineNumber_, token_type::string_type());
            }
            else if (isnewline(firstChar))      //  Line endings
            {
                tokenEnd = ParseEOLToken(inputBegin, inputEnd, outputToken);
            }
            else if (isspace(firstChar))        //  Spaces
            {
                tokenEnd = ParseWhitespaceToken(inputBegin, inputEnd, outputToken);
            }
            else if (isstrbegin(firstChar))     //  String literal
            {
                tokenEnd = ParseStringToken(inputBegin, inputEnd, outputToken);
            }
            else if (std::isdigit(firstChar))   //  Integer literal
            {
                // This needs to work a lot differently. It doesn't take into account negative numbers.
                tokenEnd = ParseIntegerToken(inputBegin, inputEnd, outputToken);
            }
            else if (isvarstart(firstChar))     //  variable
            {
                tokenEnd = ParseVariableToken(inputBegin, inputEnd, outputToken);
            }
            else if (isidentbegin(firstChar))   //  Identifier
            {
                tokenEnd = ParseIdentifierToken(inputBegin, inputEnd, outputToken);
            }
            else if (std::ispunct(firstChar))   //  Operators
            {
                tokenEnd = ParseOperatorToken(inputBegin, inputEnd, outputToken);
            }
            else
            {
                reportingContext_.ReportError(lineNumber_, "Unknown character `", firstChar, "` in input stream.");
                std::advance(tokenEnd, 1);
                outputToken = token_type(
                    tokenid_type::Invalid,
                    lineNumber_,
                    token_type::string_type(inputBegin, tokenEnd));
            }

            lineNumber_ += std::count_if(
                inputBegin, tokenEnd,
                [this](char_type value) { return this->isnewline(value); });

            return tokenEnd;
        }


    protected:

        virtual bool isspace(char_type ch) const { return ch == char_type(' ') || ch == char_type('\t'); }
        virtual bool isnewline(char_type ch) const { return ch == char_type('\n'); }
        virtual bool isstrbegin(char_type ch) const { return ch == char_type('"'); }
        virtual bool isstrend(char_type ch) const { return ch == char_type('"'); }
        virtual bool isidentbegin(char_type ch) const { return ch == char_type('_') || std::isalpha(ch); }
        virtual bool isvarstart(char_type ch) const { return ch == char_type('$'); }
        virtual bool isvarbegin(char_type ch) const { return isidentbegin(ch); }
        virtual bool isvarchar(char_type ch) const { return isident(ch) || ch == char_type('.'); }
        virtual bool isident(char_type ch) const { return ch == char_type('_') || std::isalnum(ch); }
        virtual bool iscommentchar(char_type ch) const { return ch == char_type('#'); }


        virtual const_iterator ParseWhitespaceToken(
            const_iterator inputBegin,
            const_iterator inputEnd,
            token_type& outputToken)
        {
            const auto isspace = [this](char_type ch) { return this->isspace(ch); };
            auto spacesBegin = inputBegin;
            auto spacesEnd = std::find_if_not(inputBegin, inputEnd, isspace);  //  Skip spaces
            outputToken = token_type(
                tokenid_type::Space,
                lineNumber_,
                token_type::string_type(spacesBegin, spacesEnd),
                value_type(std::distance(spacesBegin, spacesEnd)));

            return spacesEnd;
        }

        virtual const_iterator ParseEOLToken(
            const_iterator inputBegin,
            const_iterator inputEnd,
            token_type& outputToken)
        {
            const auto isnewline = [this](char_type ch) { return this->isnewline(ch);  };
            auto eolBegin = inputBegin;
            auto eolEnd = std::find_if_not(inputBegin, inputEnd, isnewline);  //  Skip newlines
            outputToken = token_type(
                tokenid_type::LineEnd,
                lineNumber_,
                token_type::string_type(eolBegin, eolEnd),
                value_type(std::distance(eolBegin, eolEnd)));

            return eolEnd;
        }

        virtual const_iterator ParseOperatorToken(
            const_iterator inputBegin,
            const_iterator inputEnd,
            token_type& outputToken)
        {
            auto operatorBegin = inputBegin;
            auto operatoeEnd = inputBegin;
            string_type operatorText;
            while (operatoeEnd != inputEnd && operatorText.size() < maximumOperatorSize_ && ispunct(*operatoeEnd))
            {
                operatorText.push_back(*operatoeEnd);
                std::advance(operatoeEnd, 1);
            }
            operatoeEnd = operatorBegin;
            while (operatorText.size())
            {
                auto op = operators_.find(operatorText);
                if (op != operators_.end())
                {
                    std::advance(operatoeEnd, operatorText.size());
                    outputToken = token_type(op->second, lineNumber_, operatorText);
                    break;
                }

                operatorText.pop_back();
            }

            return operatoeEnd;
        }

        virtual const_iterator ParseIdentifierToken(
            const_iterator inputBegin,
            const_iterator inputEnd,
            token_type& outputToken)
        {
            auto identifierBegin = inputBegin;
            auto identifierEnd = inputBegin;

            //  If this is a beginning character skip it.
            if (isidentbegin(*identifierEnd))
            {
                std::advance(identifierEnd, 1);
            }

            identifierEnd = std::find_if_not(
                identifierEnd, inputEnd,
                [this](char_type ch) -> bool { return this->isident(ch); });

            const token_type::string_type identifier(identifierBegin, identifierEnd);
            auto word = reservedWords_.find(identifier);
            if (word != reservedWords_.end())
            {
                outputToken = token_type(word->second, lineNumber_, identifier);
            }
            else
            {
                outputToken = token_type(
                    tokenid_type::Identifier,
                    lineNumber_,
                    identifier,
                    value_type(value_type::Identifier, identifier));
            }

            return identifierEnd;
        }

        virtual const_iterator ParseVariableToken(
            const_iterator inputBegin,
            const_iterator inputEnd,
            token_type& outputToken)
        {
            auto identifierBegin = inputBegin;
            auto identifierEnd = inputBegin;

            //  If this is a beginning character skip it.
            if (isvarstart(*identifierEnd) == false)
            {
                throw std::runtime_error("This is not a variable!");
            }

            std::advance(identifierEnd, 1);
            if (isvarbegin(*identifierEnd))
            {
                auto isvarchar = [this](char_type ch) -> bool { return this->isvarchar(ch); };
                identifierEnd = std::find_if_not(identifierEnd, inputEnd, isvarchar);
            }

            const token_type::string_type identifier(identifierBegin, identifierEnd);
            outputToken = token_type(
                tokenid_type::Variable,
                lineNumber_,
                identifier,
                value_type(value_type::Variable, identifier));
            
            return identifierEnd;
        }


        virtual const_iterator ParseStringToken(
            const_iterator inputBegin,
            const_iterator inputEnd,
            token_type& outputToken)
        {
            const auto isstrend = [this](char_type ch) { return this->isstrend(ch);  };

            auto stringBegin = inputBegin;
            std::advance(stringBegin, 1);
            auto stringEnd = std::find_if(stringBegin, inputEnd, isstrend);
            outputToken = token_type(
                tokenid_type::String,
                lineNumber_,
                token_type::string_type(stringBegin, stringEnd),
                value_type(value_type::String, token_type::string_type(stringBegin, stringEnd)));
            if (stringEnd == inputEnd)
            {
                reportingContext_.ReportError(lineNumber_, "Unterminated string literal");
            }
            else if (stringEnd != inputEnd && isstrend(*stringEnd))
            {
                std::advance(stringEnd, 1);
            }


            return stringEnd;
        }

        virtual const_iterator ParseIntegerToken(
            const_iterator inputBegin,
            const_iterator inputEnd,
            token_type& outputToken)
        {
            std::uint64_t value = 0;

            auto integerEnd = inputBegin;
            while (integerEnd != inputEnd && std::isdigit(*integerEnd))
            {
                value = (value * 10) + (*integerEnd - '0');
                ++integerEnd;
            }

            outputToken = token_type(
                tokenid_type::Integer,
                lineNumber_,
                token_type::string_type(inputBegin, integerEnd),
                value_type(value));
            
            return integerEnd;
        }


    protected:

        static const std::size_t maximumOperatorSize_ = 5;

        ErrorReporting&                         reportingContext_;
        std::size_t                             lineNumber_;
        std::map<string_type, tokenid_type>    reservedWords_;
        std::map<string_type, tokenid_type>    operators_;
        std::map<tokenid_type, string_type>    tokenNames_;
    };

}

