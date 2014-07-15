#include "Lexer.h"
#include <cctype>
#include <iostream>

//namespace std
//{
//    template<class _Ty, class... _Types>
//    inline unique_ptr<_Ty> make_unique(_Types&&... _Args)
//    {
//        return unique_ptr<_Ty>(new _Ty(std::forward<_Types>(_Args)...));
//    }
//}


//namespace Decals
//{
//    enum class Token
//    {
//        Invalid,
//        InputEnd, Space, LineEnd,
//        Operator, Keyword,
//        Identifier, String, Integer,
//        //
//        Tilde,
//        BackTick,
//        Bang,   //  Excalmation mark
//        At,
//        Hash,
//        Dollar,
//        Percent,
//        Caret,
//        Ampersand,
//        Asterisk,
//        CurlyBracket_Open,      //  Parenthesis
//        CurlyBracket_Close,     //  Parenthesis
//        Hyphen,
//        Equal,
//        Plus,
//        SquareBracket_Open,
//        SquareBracket_Close,
//        CurlyBrace_Open,
//        CurlyBrace_Close,
//        Backslash,
//        Pipe,
//        Colon,
//        Semicolon,
//        SingleQuote,
//        DoubleQuote,
//        Comma,
//        Period,
//        AngleBrackets_Open,
//        AngleBrackets_Close,
//        Slash,
//        Question,
//
//        //
//        If,
//        Then,
//        Else,
//        ElseIf,
//        EndIf,
//        For,
//        ForNext,
//        LogicalNot,
//        LogicalAnd,
//        LogicalOr,
//        LogicalXor
//    };
//}



//reservedWords_.insert(std::make_pair(L"hello", keyword_type(1)));
//reservedWords_.insert(std::make_pair(L"world", keyword_type(2)));
//operators_.insert(std::make_pair(L"~", Operator::Tilde));
//operators_.insert(std::make_pair(L"`", Operator::BackTick));
//operators_.insert(std::make_pair(L"!", Operator::Bang));
//operators_.insert(std::make_pair(L"@", Operator::At));
//operators_.insert(std::make_pair(L"#", Operator::Hash));
//operators_.insert(std::make_pair(L"$", Operator::Dollar));
//operators_.insert(std::make_pair(L"%", Operator::Percent));
//operators_.insert(std::make_pair(L"^", Operator::Caret));
//operators_.insert(std::make_pair(L"&", Operator::Ampersand));
//operators_.insert(std::make_pair(L"*", Operator::Asterisk));
//operators_.insert(std::make_pair(L"(", Operator::CurlyBracket_Open));
//operators_.insert(std::make_pair(L")", Operator::CurlyBracket_Close));
//operators_.insert(std::make_pair(L"-", Operator::Hyphen));
//operators_.insert(std::make_pair(L"=", Operator::Equal));
//operators_.insert(std::make_pair(L"+", Operator::Plus));
//operators_.insert(std::make_pair(L"[", Operator::SquareBracket_Open));
//operators_.insert(std::make_pair(L"]", Operator::SquareBracket_Close));
//operators_.insert(std::make_pair(L"(", Operator::CurlyBrace_Open));
//operators_.insert(std::make_pair(L")", Operator::CurlyBrace_Close));
//operators_.insert(std::make_pair(L"\\", Operator::Backslash));
//operators_.insert(std::make_pair(L"|", Operator::Pipe));
//operators_.insert(std::make_pair(L";", Operator::Semicolon));
//operators_.insert(std::make_pair(L":", Operator::Colon));
//operators_.insert(std::make_pair(L"'", Operator::SingleQuote));
//operators_.insert(std::make_pair(L"\"", Operator::DoubleQuote));
//operators_.insert(std::make_pair(L",", Operator::Comma));
//operators_.insert(std::make_pair(L".", Operator::Period));
//operators_.insert(std::make_pair(L"<", Operator::AngleBrackets_Open));
//operators_.insert(std::make_pair(L">", Operator::AngleBrackets_Close));
//operators_.insert(std::make_pair(L"/", Operator::Slash));
//operators_.insert(std::make_pair(L"?", Operator::Question));


/*

//Token           75
//Lexer           300
//TokenSequence   250
//Diagnostics     75

enum class TokenId
{
    Invalid,
    InputEnd, Space, LineEnd,
    Operator,
    Identifier, String, Integer,

    //
    CurlyBracket_Open, CurlyBracket_Close,
    Assign,
    Plus, Minus, Divide,
};


typedef TokenT<TokenId> Token;
typedef LexerT<Token> Lexer;


struct Node
{
    explicit Node(const Token& token) : token(token) {}
    explicit Node(Token&& token) : token(token) {}

    void Dump(std::size_t depth) const
    {
        if (lvalue) lvalue->Dump(depth + token.getText().size() + 5);
        std::wcout << std::right << std::setw(depth) << "" << "- " << token.getText();
        if (lvalue || rvalue)
        {
            std::wcout << " -<";
        }
        std::wcout << std::endl;
        if (rvalue) rvalue->Dump(depth + token.getText().size() + 5);
    }

    Token                   token;
    std::unique_ptr<Node>   lvalue;
    std::unique_ptr<Node>   rvalue;
};

std::unique_ptr<Node> ParseExpression(TokenSequence<Lexer>& stream)
{

    //  TODO: Handle unary ops
    if (stream.PeekTokenId() == Lexer::tokenid_type::CurlyBracket_Open)
    {
        stream.Next();
        auto expression = ParseExpression(stream);
        if (stream.Expect(Lexer::tokenid_type::CurlyBracket_Close) == false)
        {
            throw syntax_error("Expected ')'");
        }

        return expression;
    }

    //  Parse something something dark side...
    switch (stream.PeekTokenId())
    {
    case Lexer::tokenid_type::Identifier:
    case Lexer::tokenid_type::Integer:
    case Lexer::tokenid_type::String:
        break;

    default:
        throw syntax_error("Expected an expression");
    }

    std::unique_ptr<Node>   expression(new Node(stream.Pop()));

    //  Parse the operator if present
    if (stream.hasTokens(1))
    {
        bool isBinary = false;
        switch (stream.PeekTokenId())
        {
        case Lexer::tokenid_type::Plus:
        case Lexer::tokenid_type::Minus:
        case Lexer::tokenid_type::Divide:
            isBinary = true;
        }
        if (isBinary)
        {
            std::unique_ptr<Node>   lvalue(new Node(stream.Pop()));
            expression.swap(lvalue->lvalue);
            expression.swap(lvalue);
            expression->rvalue = ParseExpression(stream);
        }
    }



    return expression;
}



int main()
{
    static const std::map<Lexer::tokenid_type, std::wstring> tokenNames
    {
        { Lexer::tokenid_type::Invalid, L"******" },
        { Lexer::tokenid_type::InputEnd, L"<EOI.>" },
        { Lexer::tokenid_type::Space, L"<SPCE>" },
        { Lexer::tokenid_type::LineEnd, L"<EOL.>" },
        { Lexer::tokenid_type::Operator, L"<OPER>" },
        { Lexer::tokenid_type::Identifier, L"<IDNT>" },
        { Lexer::tokenid_type::String, L"<STR.>" },
        { Lexer::tokenid_type::Integer, L"<INT.>" },
        //
        { Lexer::tokenid_type::CurlyBracket_Open, L"oparnt" },
        { Lexer::tokenid_type::CurlyBracket_Close, L"cparnt" },
        { Lexer::tokenid_type::Assign, L"assign" },
        { Lexer::tokenid_type::Plus, L"plus" },
        { Lexer::tokenid_type::Minus, L"minus" },
        { Lexer::tokenid_type::Divide, L"divide" },
    };

    MachineEx machine;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 1);
    machine.genOp(Machine::Opcode::PushValue, 1);
    machine.genOp(Machine::Opcode::Equals);
    machine.genOp(Machine::Opcode::Return);
    std::cout << " true = " << machine.execute_call(0) << std::endl;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 1);
    machine.genOp(Machine::Opcode::PushValue, 2);
    machine.genOp(Machine::Opcode::Equals);
    machine.genOp(Machine::Opcode::Return);
    std::cout << "false = " << machine.execute_call(0) << std::endl;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 2);
    machine.genOp(Machine::Opcode::PushValue, 1);
    machine.genOp(Machine::Opcode::LessThan);
    machine.genOp(Machine::Opcode::Return);
    std::cout << " true = " << machine.execute_call(0) << std::endl;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 1);
    machine.genOp(Machine::Opcode::PushValue, 2);
    machine.genOp(Machine::Opcode::LessThan);
    machine.genOp(Machine::Opcode::Return);
    std::cout << "false = " << machine.execute_call(0) << std::endl;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 1);
    machine.genOp(Machine::Opcode::PushValue, 2);
    machine.genOp(Machine::Opcode::GreaterThan);
    machine.genOp(Machine::Opcode::Return);
    std::cout << " true = " << machine.execute_call(0) << std::endl;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 2);
    machine.genOp(Machine::Opcode::PushValue, 1);
    machine.genOp(Machine::Opcode::GreaterThan);
    machine.genOp(Machine::Opcode::Return);
    std::cout << "false = " << machine.execute_call(0) << std::endl;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 2);
    machine.genOp(Machine::Opcode::PushValue, 6);
    machine.genOp(Machine::Opcode::Multiply);
    machine.genOp(Machine::Opcode::Return);
    std::cout << "12 = " << machine.execute_call(0) << std::endl;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 2);
    machine.genOp(Machine::Opcode::PushValue, 6);
    machine.genOp(Machine::Opcode::Divide);
    machine.genOp(Machine::Opcode::Return);
    std::cout << " 3 = " << machine.execute_call(0) << std::endl;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 6);
    machine.genOp(Machine::Opcode::PushValue, 2);
    machine.genOp(Machine::Opcode::Add);
    machine.genOp(Machine::Opcode::Return);
    std::cout << " 8 = " << machine.execute_call(0) << std::endl;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 6);
    machine.genOp(Machine::Opcode::PushValue, 2);
    machine.genOp(Machine::Opcode::Subtract);
    machine.genOp(Machine::Opcode::Return);
    std::cout << "-4 = " << machine.execute_call(0) << std::endl;

    machine.reset();
    machine.genOp(Machine::Opcode::PushValue, 3);
    machine.genOp(Machine::Opcode::PushValue, 8);
    machine.genOp(Machine::Opcode::Modulo);
    machine.genOp(Machine::Opcode::Return);
    std::cout << " 2 = " << machine.execute_call(0) << std::endl;


    Lexer lex;
    lex.AddOperator(L"(", Lexer::tokenid_type::CurlyBracket_Open);
    lex.AddOperator(L")", Lexer::tokenid_type::CurlyBracket_Close);
    lex.AddOperator(L"=", Lexer::tokenid_type::Assign);
    lex.AddOperator(L"+", Lexer::tokenid_type::Plus);
    lex.AddOperator(L"-", Lexer::tokenid_type::Minus);
    lex.AddOperator(L"/", Lexer::tokenid_type::Divide);

    auto tokens = lex.Tokenize(L"mother_child + trude / 1 / (x + y)");
    tokens.erase(std::remove(tokens.begin(), tokens.end(), Lexer::tokenid_type::LineEnd), tokens.end());
    tokens.erase(std::remove(tokens.begin(), tokens.end(), Lexer::tokenid_type::Space), tokens.end());
    DumpTokens(tokens, tokenNames);

    TokenSequence<Lexer> stream(tokens.begin(), tokens.end());
    auto result = ParseExpression(stream);
    result->Dump(4);

    int captured = 0;
    //auto x = [&](){ return tokens;  };
    auto x = [captured]() { return captured;  };

    typedef decltype(x) bar;
    //struct bar { int operator()() const { return 0; } };



    struct foo : protected bar
    {
    public:
        //foo(decltype(tokens)& t) : bar(t) {}
        foo() : bar(1) {}
        void operator()() const
        {
            bar::operator()();
        }
    };

    foo f;
    f();

}


*/



//
//static const std::map<Lexer::tokenid_type, std::wstring> tokenNames
//{
//    { Lexer::tokenid_type::Invalid, L"******" },
//    { Lexer::tokenid_type::InputEnd, L"<EOI.>" },
//    { Lexer::tokenid_type::Space, L"<SPCE>" },
//    { Lexer::tokenid_type::LineEnd, L"<EOL.>" },
//    { Lexer::tokenid_type::Operator, L"<OPER>" },
//    { Lexer::tokenid_type::Identifier, L"<IDNT>" },
//    { Lexer::tokenid_type::String, L"<STR.>" },
//    { Lexer::tokenid_type::Integer, L"<INT.>" },
//    //
//    { Lexer::tokenid_type::Click, L"Click" },
//    { Lexer::tokenid_type::Decrease, L"Decrease" },
//    { Lexer::tokenid_type::Increase, L"Increase" },
//    { Lexer::tokenid_type::Enable, L"Enable" },
//    { Lexer::tokenid_type::Disable, L"Disable" },
//    { Lexer::tokenid_type::Insert, L"Insert" },
//    { Lexer::tokenid_type::Select, L"Select" },
//    { Lexer::tokenid_type::Request, L"Request" },
//    { Lexer::tokenid_type::Trigger, L"Trigger" },
//    { Lexer::tokenid_type::Update, L"Update" },
//    { Lexer::tokenid_type::Load, L"Load" },
//    //  Operators
//    { Lexer::tokenid_type::CurlyBracket_Open, L"oparnt" },
//    { Lexer::tokenid_type::CurlyBracket_Close, L"cparnt" },
//    { Lexer::tokenid_type::Assign, L"assign" },
//    { Lexer::tokenid_type::Plus, L"plus" },
//    { Lexer::tokenid_type::Minus, L"minus" },
//    { Lexer::tokenid_type::Divide, L"divide" },
//};


