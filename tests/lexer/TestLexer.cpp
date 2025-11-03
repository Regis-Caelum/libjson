#include "lexer/Lexer.h"

#include <gtest/gtest.h>

static std::vector<Token> tokenize(std::string input)
{
    Lexer lexer(input);
    std::vector<Token> tokens;
    for (;;)
    {
        Token t = lexer.nextToken();
        tokens.push_back(t);
        if (t.type == Token::EOFTOKEN)
            break;
    }
    return tokens;
}

// --- BASIC TOKEN TESTS ---

TEST(LexerTest, PunctuationTokens)
{
    Lexer lexer("{ } [ ] : ,");
    EXPECT_EQ(lexer.nextToken().type, Token::LBRACE);
    EXPECT_EQ(lexer.nextToken().type, Token::RBRACE);
    EXPECT_EQ(lexer.nextToken().type, Token::LBRACKET);
    EXPECT_EQ(lexer.nextToken().type, Token::RBRACKET);
    EXPECT_EQ(lexer.nextToken().type, Token::COLON);
    EXPECT_EQ(lexer.nextToken().type, Token::COMMA);
    EXPECT_EQ(lexer.nextToken().type, Token::EOFTOKEN);
}

// --- STRING TESTS ---

TEST(LexerTest, SimpleString)
{
    Lexer lexer("\"hello\"");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::STRING);
    EXPECT_EQ(t.lexeme, "hello");
}

TEST(LexerTest, EscapedString)
{
    Lexer lexer("\"hello \\\"world\\\"\"");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::STRING);
    EXPECT_EQ(t.lexeme, "hello \"world\"");
}

TEST(LexerTest, UnterminatedString)
{
    Lexer lexer("\"unterminated");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::INVALID);
}

// --- NUMBER TESTS ---

TEST(LexerTest, IntegerNumber)
{
    Lexer lexer("12345");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::INTEGER);
    EXPECT_EQ(t.lexeme, "12345");
}

TEST(LexerTest, NegativeInteger)
{
    Lexer lexer("-42");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::INTEGER);
    EXPECT_EQ(t.lexeme, "-42");
}

TEST(LexerTest, FloatNumber)
{
    Lexer lexer("3.14");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::FLOAT);
    EXPECT_EQ(t.lexeme, "3.14");
}

TEST(LexerTest, ExponentialNumber)
{
    Lexer lexer("1.23e-4");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::FLOAT);
    EXPECT_EQ(t.lexeme, "1.23e-4");
}

TEST(LexerTest, InvalidNumber)
{
    Lexer lexer("12.");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::INVALID);
}

// --- LITERAL TESTS ---

TEST(LexerTest, TrueLiteral)
{
    Lexer lexer("true");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::TRUE);
    EXPECT_EQ(t.lexeme, "true");
}

TEST(LexerTest, FalseLiteral)
{
    Lexer lexer("false");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::FALSE);
    EXPECT_EQ(t.lexeme, "false");
}

TEST(LexerTest, NullLiteral)
{
    Lexer lexer("null");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::NULLTOKEN);
    EXPECT_EQ(t.lexeme, "null");
}

TEST(LexerTest, InvalidLiteral)
{
    Lexer lexer("truely");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::INVALID);
}

// --- WHITESPACE & MIXED INPUT ---

TEST(LexerTest, SkipWhitespace)
{
    Lexer lexer("   \n\t 123 ");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::INTEGER);
    EXPECT_EQ(t.lexeme, "123");
}

TEST(LexerTest, MixedTokens)
{
    Lexer lexer("{ \"key\": 42, \"flag\": true }");
    std::vector<Token> tokens = tokenize(lexer.input());
    std::vector<Token::Type> expected = {
        Token::LBRACE, Token::STRING, Token::COLON, Token::INTEGER,
        Token::COMMA, Token::STRING, Token::COLON, Token::TRUE,
        Token::RBRACE, Token::EOFTOKEN};

    ASSERT_EQ(tokens.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i)
        EXPECT_EQ(tokens[i].type, expected[i]) << "Mismatch at token " << i;
}

// --- END OF INPUT & INVALID CHARACTER ---

TEST(LexerTest, EndOfInput)
{
    Lexer lexer("");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::EOFTOKEN);
}

TEST(LexerTest, InvalidCharacter)
{
    Lexer lexer("@");
    Token t = lexer.nextToken();
    EXPECT_EQ(t.type, Token::INVALID);
    EXPECT_EQ(t.lexeme, "@");
}
