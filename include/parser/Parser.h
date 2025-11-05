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

bool Parser::validate()
{
    reset();
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