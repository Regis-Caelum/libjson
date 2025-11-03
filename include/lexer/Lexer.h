#ifndef LEXER_H
#define LEXER_H

#include "Token.h"

#include <string>

class Lexer
{
public:
    Lexer(std::string_view input) : input_(input), pos_(0) {}

    std::string input() const { return input_; }

    void skipWhitespace();
    Token nextToken();

    Token parseString();
    Token parseNumber();
    Token parseLiteral();

private:
    std::string input_;
    size_t pos_;

    char peek() const { return pos_ < input_.size() ? input_[pos_] : '\0'; }
    char get() { return pos_ < input_.size() ? input_[pos_++] : '\0'; }
    bool eof() const { return pos_ >= input_.size(); }
};

inline void Lexer::skipWhitespace()
{
    while (!eof())
    {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            get();
        else
            break;
    }
}

inline Token Lexer::nextToken()
{
    skipWhitespace();
    if (eof())
        return Token(Token::EOFTOKEN, "", pos_);

    char c = peek();

    switch (c)
    {
    case '{':
        get();
        return Token(Token::LBRACE, "{", pos_ - 1);
    case '}':
        get();
        return Token(Token::RBRACE, "}", pos_ - 1);
    case '[':
        get();
        return Token(Token::LBRACKET, "[", pos_ - 1);
    case ']':
        get();
        return Token(Token::RBRACKET, "]", pos_ - 1);
    case ',':
        get();
        return Token(Token::COMMA, ",", pos_ - 1);
    case ':':
        get();
        return Token(Token::COLON, ":", pos_ - 1);
    case '"':
        return parseString();
    case 't':
    case 'f':
    case 'n':
        return parseLiteral();
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return parseNumber();
    default:
        get();
        return Token(Token::INVALID, std::string(1, c), pos_ - 1);
    }
}

inline Token Lexer::parseString()
{
    std::string result;
    get();

    while (!eof())
    {
        char c = get();
        if (c == '"')
            return Token(Token::STRING, result, pos_);
        if (c == '\\')
        {
            if (eof())
                break;
            char esc = get();
            switch (esc)
            {
            case '"':
                result += '"';
                break;
            case '\\':
                result += '\\';
                break;
            case '/':
                result += '/';
                break;
            case 'b':
                result += '\b';
                break;
            case 'f':
                result += '\f';
                break;
            case 'n':
                result += '\n';
                break;
            case 'r':
                result += '\r';
                break;
            case 't':
                result += '\t';
                break;
            default:
                result += esc;
                break;
            }
        }
        else
        {
            result += c;
        }
    }

    return Token(Token::INVALID, result, pos_);
}

inline Token Lexer::parseNumber()
{
    std::string result;

    if (peek() == '-')
        result += get();

    bool hasDigits = false;

    while (!eof() && std::isdigit(peek()))
    {
        result += get();
        hasDigits = true;
    }

    bool isFloat = false;

    if (!eof() && peek() == '.')
    {
        isFloat = true;
        result += get();
        if (eof() || !std::isdigit(peek()))
            return Token(Token::INVALID, result, pos_);
        while (!eof() && std::isdigit(peek()))
            result += get();
    }

    if (!eof() && (peek() == 'e' || peek() == 'E'))
    {
        isFloat = true;
        result += get();
        if (!eof() && (peek() == '+' || peek() == '-'))
            result += get();
        if (eof() || !std::isdigit(peek()))
            return Token(Token::INVALID, result, pos_);
        while (!eof() && std::isdigit(peek()))
            result += get();
    }

    if (!hasDigits)
        return Token(Token::INVALID, result, pos_);

    return isFloat
               ? Token(Token::FLOAT, result, pos_)
               : Token(Token::INTEGER, result, pos_);
}

inline Token Lexer::parseLiteral()
{
    size_t start = pos_;
    std::string lexeme;

    while (!eof() && std::isalpha(peek()))
        lexeme += get();

    if (lexeme == "true")
        return Token(Token::TRUE, lexeme, start);
    if (lexeme == "false")
        return Token(Token::FALSE, lexeme, start);
    if (lexeme == "null")
        return Token(Token::NULLTOKEN, lexeme, start);

    return Token(Token::INVALID, lexeme, start);
}

#endif // LEXER_H