#ifndef PARSER_H
#define PARSER_H

#include "lexer/Token.h"
#include "json/Json.h"

#include <vector>
#include <stack>
#include <variant>

class Parser
{
public:
    Parser(std::vector<Token> tokens) : tokens_(tokens), pos_(0) {}

    std::vector<Token> tokens() const { return tokens_; }

    Json buildJson();

    bool validate();

private:
    std::vector<Token> tokens_;
    size_t pos_;

    Token current();
    void consume();
    void reset() { pos_ = 0; }

    JsonValue parseValue(Token t);
    JsonValue::string_t parseString(Token t);
    JsonValue::number_float_t parseFloat(Token t);
    JsonValue::number_integer_t parseInteger(Token t);
    JsonValue::object_t parseObject(Token t);
    JsonValue::array_t parseArray(Token t);
    JsonValue::boolean_t parseBoolean(Token t);
    JsonValue::nullptr_t parseNull(Token t);
};

inline Token Parser::current()
{
    if (pos_ < tokens_.size())
        return tokens_[pos_];

    return Token(Token::EOFTOKEN);
}

inline void Parser::consume()
{
    if (pos_ < tokens_.size())
        pos_++;
}

inline bool Parser::validate()
{
    reset();
    Token prev = tokens_.front();
    std::stack<Token> s;
    Token first = tokens_.front();
    Token last = tokens_.back();

    if (first.type != Token::LBRACE && first.type != Token::LBRACKET)
        return false;

    if ((first.type == Token::LBRACE && last.type != Token::RBRACE) ||
        (first.type == Token::LBRACKET && last.type != Token::RBRACKET))
    {
        return false;
    }

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

inline Json Parser::buildJson()
{
    if (!validate())
        throw std::runtime_error("Invalid JSON");

    JsonValue v = parseValue(current());

    if (!v.is_object())
        throw std::runtime_error("Root element is not an object");

    return static_cast<JsonValue::object_t>(v);
}

inline JsonValue Parser::parseValue(Token t)
{
    consume();
    switch (t.type)
    {
    case Token::STRING:
        return parseString(t);
    case Token::INTEGER:
        return parseInteger(t);
    case Token::FLOAT:
        return parseFloat(t);
    case Token::TRUE:
        return parseBoolean(t);
    case Token::FALSE:
        return parseBoolean(t);
    case Token::LBRACE:
        return parseObject(t);
    case Token::LBRACKET:
        return parseArray(t);
    case Token::NULLTOKEN:
        return parseNull(t);
    default:
        std::runtime_error("Invalid token");
    }
}

inline JsonValue::string_t
Parser::parseString(Token t)
{
    consume();
    return t.lexeme;
}

inline JsonValue::number_integer_t Parser::parseInteger(Token t)
{
    consume();
    return std::stoi(t.lexeme);
}

inline JsonValue::number_float_t Parser::parseFloat(Token t)
{
    consume();
    return std::stod(t.lexeme);
}

inline JsonValue::boolean_t Parser::parseBoolean(Token t)
{
    consume();
    return t.lexeme == "true";
}

inline JsonValue::nullptr_t Parser::parseNull(Token t)
{
    consume();
    return nullptr;
}

inline JsonValue::object_t Parser::parseObject(Token t)
{
    consume();
    JsonValue::object_t j;
    while (current().type != Token::RBRACE)
    {
        Token key = current();
        if (key.type != Token::STRING)
            throw std::runtime_error("Expected string as key in object");

        consume();

        if (current().type != Token::COLON)
            throw std::runtime_error("Expected ':' after key");
        consume();

        j[key.lexeme] = parseValue(current());

        if (current().type == Token::COMMA)
            consume();
        else if (current().type != Token::RBRACE)
            throw std::runtime_error("Expected ',' or '}' in object");
    }
    consume();
    return j;
}

inline JsonValue::array_t Parser::parseArray(Token t)
{
    consume();
    JsonValue::array_t arr;

    while (current().type != Token::RBRACKET)
    {
        arr.push_back(parseValue(current()));

        if (current().type == Token::COMMA)
            consume();
        else if (current().type != Token::RBRACKET)
            throw std::runtime_error("Expected ',' or ']' in array");
    }

    consume();
    return arr;
}

#endif // PARSER_H