#pragma once
#include <DerpLex/Derp.h>
#include <sstream>


namespace Derp
{

    template<class LexerType>
    struct TokenSequence
    {
        typedef typename LexerType::token_type      token_type;
        typedef typename LexerType::output_type     input_type;
        typedef typename token_type::tokenid_type   tokenid_type;
        typedef typename input_type::const_iterator const_iterator;
        typedef typename input_type::size_type      size_type;
        typedef typename token_type::string_type    string_type;


        TokenSequence(const LexerType& lexer, ErrorReporting& reporting, const_iterator inputBegin, const_iterator inputEnd)
            : lexer_(lexer), reporting_(reporting), inputBegin_(inputBegin), inputEnd_(inputEnd), currentToken_(inputBegin)
        {
        }

        virtual ~TokenSequence()
        {
        }


        virtual size_type getTokenCount() const
        {
            return std::distance(currentToken_, inputEnd_);
        }

        virtual bool hasToken() const
        {
            return getTokenCount() > 0;
        }


        virtual token_type Pop()
        {
            if (hasToken() == false)
            {
                throw std::invalid_argument("no tokens left brah!");
            }

            return *(currentToken_++);
        }




        /////////////////////////////////////////////////////////////////////////////////
        //
        //
        //
        /////////////////////////////////////////////////////////////////////////////////

        virtual bool Peek(tokenid_type tokenId) const
        {
            return hasToken() ? currentToken_->getId() == tokenId : false;
        }

        virtual tokenid_type Peek() const
        {
            return hasToken() ? currentToken_->getId() : tokenid_type::Invalid;
        }

        virtual token_type PeekToken() const
        {
            return hasToken()
                ? *currentToken_
                : inputBegin_ != inputEnd_
                    ? *(currentToken_ - 1)
                    : token_type(tokenid_type::Invalid, 0, L"");
        }

        virtual bool Skip(tokenid_type tokenId)
        {
            bool result = false;
            while (hasToken() && Peek() == tokenId)
            {
                result = true;
                Pop();
            }

            return result;
        }




        /////////////////////////////////////////////////////////////////////////////////
        //
        //
        //
        /////////////////////////////////////////////////////////////////////////////////

        virtual bool Test(tokenid_type tokenId)
        {
            token_type outputToken;
            return hasToken() ? internal_Test(tokenId, outputToken) : false;
        }

        virtual bool Test(tokenid_type tokenId, token_type& outputToken)
        {
            return hasToken() ? internal_Test(tokenId, outputToken) : false;
        }

        virtual bool Test(const std::initializer_list<tokenid_type>& tokenidList, token_type& outputToken)
        {
            if (hasToken())
            {
                for (auto tokenId : tokenidList)
                {
                    if (internal_Test(tokenId, outputToken))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        virtual bool TestText(tokenid_type tokenId, const string_type& text)
        {
            token_type outputToken;
            return hasToken() ? internal_TestText(tokenId, text, outputToken) : false;
        }

        virtual bool TestText(tokenid_type tokenId, const string_type& text, token_type& outputToken)
        {
            return hasToken() ? internal_TestText(tokenId, text, outputToken) : false;
        }




        /////////////////////////////////////////////////////////////////////////////////
        //
        //
        //
        /////////////////////////////////////////////////////////////////////////////////

        virtual bool Expect(tokenid_type tokenId)
        {
            token_type outputToken;
            return hasToken() ? internal_Expect(tokenId, outputToken) : false;
        }

        virtual bool Expect(tokenid_type tokenId, token_type& outputToken)
        {
            return hasToken() ? internal_Expect(tokenId, outputToken) : false;
        }

        virtual bool Expect(const std::initializer_list<tokenid_type>& tokenidList)
        {
            token_type outputToken;
            return Expect(tokenidList, outputToken);
        }

        virtual bool Expect(const std::initializer_list<tokenid_type>& tokenidList, token_type& outputToken)
        {
            if (hasToken() && tokenidList.size())
            {
                for (auto tokenId : tokenidList)
                {
                    if (Test(tokenId, outputToken))
                    {
                        return true;
                    }
                }

                string_type tokenString;
                size_t countDown = tokenidList.size() - 1;
                for (auto tokenId : tokenidList)
                {
                    tokenString += L"`" + lexer_.getTokenName(tokenId) + L"`";
                    
                    if (countDown > 1)
                    {
                        tokenString += L", ";
                    }
                    else if (countDown == 1)
                    {
                        tokenString += L", or ";
                    }
                    --countDown;
                }

                reporting_.ReportError(currentToken_->getLine(), L"Expected " + tokenString);
            }

            return false;
        }

        virtual bool ExpectText(tokenid_type tokenId, const string_type& text, token_type& outputToken)
        {
            return hasToken() ? internal_ExpectText(tokenId, text, outputToken) : false;
        }

        virtual bool ExpectText(tokenid_type tokenId, const string_type& text)
        {
            token_type outputToken;

            return hasToken() ? internal_ExpectText(tokenId, text, outputToken) : false;
        }


    protected:

        virtual bool internal_Test(tokenid_type tokenId, token_type& outputToken)
        {
            if (currentToken_->getId() == tokenId)
            {
                outputToken = *currentToken_;
                internal_Next();

                return true;
            }

            return false;
        }

        virtual bool internal_TestText(tokenid_type tokenId, const string_type& text, token_type& outputToken)
        {
            if (currentToken_->getId() == tokenId && currentToken_->getText() == text)
            {
                outputToken = *currentToken_;
                internal_Next();
                return true;
            }

            return false;
        }

        virtual bool internal_Expect(tokenid_type tokenId, token_type& outputToken)
        {
            if (internal_Test(tokenId, outputToken))
            {
                return true;
            }

            reporting_.ReportError(currentToken_->getLine(), L"Expected `" + lexer_.getTokenName(tokenId) + L"`");
            return false;
        }

        virtual bool internal_ExpectText(tokenid_type tokenId, const string_type& text, token_type& outputToken)
        {
            if (currentToken_->getId() != tokenId)
            {
                reporting_.ReportError(currentToken_->getLine(), L"Expected `" + lexer_.getTokenName(tokenId) + L"`");
                return false;
            }

            if (currentToken_->getText() != text)
            {
                reporting_.ReportError(currentToken_->getLine(), L"Expected `" + text + L"` following `" + lexer_.getTokenName(tokenId) + L"`");
                return false;
            }

            outputToken = *currentToken_;
            internal_Next();
            return true;
        }

        virtual void internal_Next()
        {
            currentToken_++;
        }

        TokenSequence& operator=(const TokenSequence&) = delete;

    private:

        const LexerType&    lexer_;
        ErrorReporting&     reporting_;
        const_iterator      currentToken_;
        const_iterator      inputBegin_;
        const_iterator      inputEnd_;
    };

}
