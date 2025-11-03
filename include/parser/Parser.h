#ifndef PARSER_H
#define PARSER_H

#include "lexer/Token.h"

#include <vector>
#include <stack>

class Parser
{
public:
    Parser(std::vector<Token> tokens) : tokens_(tokens), pos_(0) {}

    std::vector<Token> tokens() const { return tokens_; }

    bool validate();

private:
    std::vector<Token> tokens_;
    size_t pos_;

    Token current();
    void consume();
};

Token Parser::current()
{
    if (pos_ < tokens_.size())
        return tokens_[pos_];

    return Token(Token::EOFTOKEN);
}

void Parser::consume()
{
    if (pos_ < tokens_.size())
        pos_++;
}

bool Parser::validate()
{
    if (tokens_.empty())
        return false;

    struct Context
    {
        Token::Type type;
        bool expectValue;
        bool expectColon;
        bool expectComma;
    };

    std::stack<Context> s;
    pos_ = 0;

    while (pos_ < tokens_.size())
    {
        Token t = current();
        consume();

        if (s.empty())
        {
            if (t.type != Token::LBRACE && t.type != Token::LBRACKET)
                return false;
            s.push({t.type, t.type == Token::LBRACKET, false, false});
            continue;
        }

        Context &ctx = s.top();

        switch (t.type)
        {
        case Token::LBRACE:
        case Token::LBRACKET:
            if (!ctx.expectValue)
                return false;
            s.push({t.type, t.type == Token::LBRACKET, false, false});
            ctx.expectValue = false;
            ctx.expectComma = true;
            break;

        case Token::RBRACE:
            if (ctx.type != Token::LBRACE)
                return false;
            if (ctx.expectValue && !ctx.expectColon)
                return false;
            s.pop();
            if (!s.empty())
            {
                Context &parent = s.top();
                parent.expectValue = false;
                parent.expectComma = true;
            }
            break;

        case Token::RBRACKET:
            if (ctx.type != Token::LBRACKET)
                return false;
            if (ctx.expectValue)
                return false;
            s.pop();
            if (!s.empty())
            {
                Context &parent = s.top();
                parent.expectValue = false;
                parent.expectComma = true;
            }
            break;

        case Token::STRING:
            if (ctx.type == Token::LBRACE)
            {
                if (!ctx.expectValue)
                    return false;
                ctx.expectColon = true;
                ctx.expectValue = false;
            }
            else if (ctx.type == Token::LBRACKET)
            {
                if (!ctx.expectValue)
                    return false;
                ctx.expectValue = false;
                ctx.expectComma = true;
            }
            else
                return false;
            break;

        case Token::INTEGER:
        case Token::FLOAT:
        case Token::TRUE:
        case Token::FALSE:
        case Token::NULLTOKEN:
            if (!ctx.expectValue)
                return false;
            ctx.expectValue = false;
            ctx.expectComma = true;
            ctx.expectColon = false;
            break;

        case Token::COLON:
            if (!ctx.expectColon)
                return false;
            ctx.expectColon = false;
            ctx.expectValue = true;
            break;

        case Token::COMMA:
            if (!ctx.expectComma)
                return false;
            if (ctx.type == Token::LBRACE)
                ctx.expectValue = true;
            else if (ctx.type == Token::LBRACKET)
                ctx.expectValue = true;
            ctx.expectComma = false;
            break;

        default:
            return false;
        }
    }

    return s.empty();
}

#endif // PARSER_H