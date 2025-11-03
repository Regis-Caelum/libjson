#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token
{
public:
    enum Type
    {
        LBRACE,
        RBRACE,
        LBRACKET,
        RBRACKET,
        COMMA,
        COLON,

        INTEGER,
        FLOAT,
        STRING,
        TRUE,
        FALSE,
        NULLTOKEN,

        INVALID,
        EOFTOKEN
    };

    Type type;
    std::string lexeme;
    size_t pos;

    Token(Type type_, std::string lexeme_ = "", size_t pos_ = 0) : type(type_), lexeme(lexeme_), pos(pos_) {}
};

#endif // TOKEN_H