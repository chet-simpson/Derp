#pragma once
#include <iomanip>

template<class TokenType, class TokenMapType>
void DumpToken(const TokenType& token, const TokenMapType& tokenMap)
{
    std::wstring name(L"?????");

    auto item = tokenMap.find(token.getId());
    if (item != tokenMap.end())
    {
        name = item->second;
    }

    std::wcout
        << " " << std::left << std::setw(3) << token.getLine()
        << " : " << std::right << std::setw(8) << name
        << " - " << std::left << std::setw(20) << token.getText().substr(0, 20)
        << "   " << token.getValue()
        << std::endl;
}

template<class ContainerType, class TokenMapType>
void DumpTokens(const ContainerType& tokens, TokenMapType& tokenMap)
{
    ContainerType newTokens(tokens);
    std::cout << "Token count: " << tokens.size() << std::endl << std::endl;
    for (auto token : newTokens)
    {
        DumpToken(token, tokenMap);
    }
    std::cout << "-" << std::endl;
}
