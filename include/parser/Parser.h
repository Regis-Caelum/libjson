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
    Token prev = tokens_.front();
    std::stack<Token> s;
    if (prev.type != Token::LBRACE && prev.type != Token::LBRACKET)
        return false;

    if (prev.type != Token::RBRACE && prev.type != Token::RBRACKET)
        return false;

    if (tokens_.front().type == Token::LBRACE && tokens_.back().type != Token::RBRACE)
        return false;

    if (tokens_.front().type == Token::LBRACKET && tokens_.back().type != Token::RBRACKET)
        return false;

    while (pos_ < tokens_.size())
    {
        Token t = current();
        consume();

        if (t.type == Token::LBRACE)
            s.push(t);
        else if (t.type == Token::RBRACE)
        {
            if (s.empty() || s.top().type != Token::LBRACE)
                return false;
            s.pop();
        }
        else if (t.type == Token::LBRACKET)
            s.push(t);
        else if (t.type == Token::RBRACKET)
        {
            if (s.empty() || s.top().type != Token::LBRACKET)
                return false;
            s.pop();
        }

        prev = t;
    }

    return true;
}

#endif // PARSER_H