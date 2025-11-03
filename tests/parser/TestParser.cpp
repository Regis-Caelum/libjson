#include "parser/Parser.h"
#include <gtest/gtest.h>

// ----------------------------
// Parser::validate() Tests
// ----------------------------

TEST(ParserTest, ValidEmptyObject)
{
    Parser parser({Token(Token::LBRACE), Token(Token::RBRACE)});
    EXPECT_TRUE(parser.validate());
}

TEST(ParserTest, InvalidEmptyArray)
{
    Parser parser({Token(Token::LBRACKET), Token(Token::RBRACKET)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserTest, InvalidNested)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::LBRACKET),
                   Token(Token::RBRACKET),
                   Token(Token::RBRACE)});
    EXPECT_FALSE(parser.validate());
}

// 1. Empty input
TEST(ParserTest, InvalidEmptyInput)
{
    Parser parser({});
    EXPECT_FALSE(parser.validate());
}

// 2. Unbalanced braces
TEST(ParserTest, InvalidMissingClosingBrace)
{
    Parser parser({Token(Token::LBRACE)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserTest, InvalidMissingClosingBraceWithInner)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::STRING, "key"),
                   Token(Token::COLON),
                   Token(Token::INTEGER, "123")});
    EXPECT_FALSE(parser.validate());
}

// 3. Unbalanced brackets
TEST(ParserTest, InvalidMissingClosingBracket)
{
    Parser parser({Token(Token::LBRACKET)});
    EXPECT_FALSE(parser.validate());
}

// 4. Mismatched brackets
TEST(ParserTest, InvalidMismatchedBraces)
{
    Parser parser({Token(Token::LBRACE), Token(Token::RBRACKET)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserTest, InvalidMismatchedBrackets)
{
    Parser parser({Token(Token::LBRACKET), Token(Token::RBRACE)});
    EXPECT_FALSE(parser.validate());
}

// 5. Wrong start/end pair
TEST(ParserTest, InvalidStartsWithBraceEndsWithBracket)
{
    Parser parser({Token(Token::LBRACE), Token(Token::RBRACKET)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserTest, InvalidStartsWithBracketEndsWithBrace)
{
    Parser parser({Token(Token::LBRACKET), Token(Token::RBRACE)});
    EXPECT_FALSE(parser.validate());
}

// 6. Nested mismatches
TEST(ParserTest, InvalidNestedMismatched1)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::LBRACKET),
                   Token(Token::RBRACE), // wrong closing
                   Token(Token::RBRACKET)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserTest, InvalidNestedMismatched2)
{
    Parser parser({Token(Token::LBRACKET),
                   Token(Token::LBRACE),
                   Token(Token::RBRACKET), // wrong closing
                   Token(Token::RBRACE)});
    EXPECT_FALSE(parser.validate());
}

// 7. Extra closing token
TEST(ParserTest, InvalidExtraClosingBrace)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::RBRACE),
                   Token(Token::RBRACE)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserTest, InvalidExtraClosingBracket)
{
    Parser parser({Token(Token::LBRACKET),
                   Token(Token::RBRACKET),
                   Token(Token::RBRACKET)});
    EXPECT_FALSE(parser.validate());
}

// 8. Standalone closing token
TEST(ParserTest, InvalidStandaloneClosingBrace)
{
    Parser parser({Token(Token::RBRACE)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserTest, InvalidStandaloneClosingBracket)
{
    Parser parser({Token(Token::RBRACKET)});
    EXPECT_FALSE(parser.validate());
}

// 9. Random tokens (no brackets at all)
TEST(ParserTest, InvalidOnlyString)
{
    Parser parser({Token(Token::STRING, "hello")});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserTest, InvalidOnlyNumber)
{
    Parser parser({Token(Token::INTEGER, "42")});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserTest, InvalidOnlyLiteral)
{
    Parser parser({Token(Token::TRUE)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserInvalidTest, EmptyTokens)
{
    Parser parser({});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserInvalidTest, SingleValueWithoutObjectOrArray)
{
    Parser parser({Token(Token::INTEGER, "42")});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserInvalidTest, UnmatchedBraces)
{
    Parser parser({Token(Token::LBRACE)});
    EXPECT_FALSE(parser.validate());

    Parser parser2({Token(Token::RBRACE)});
    EXPECT_FALSE(parser2.validate());

    Parser parser3({Token(Token::LBRACE), Token(Token::LBRACE)});
    EXPECT_FALSE(parser3.validate());
}

TEST(ParserInvalidTest, UnmatchedBrackets)
{
    Parser parser({Token(Token::LBRACKET)});
    EXPECT_FALSE(parser.validate());

    Parser parser2({Token(Token::RBRACKET)});
    EXPECT_FALSE(parser2.validate());

    Parser parser3({Token(Token::LBRACKET), Token(Token::LBRACKET)});
    EXPECT_FALSE(parser3.validate());
}

TEST(ParserInvalidTest, MissingColonInObject)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::STRING, "key"),
                   Token(Token::INTEGER, "42"),
                   Token(Token::RBRACE)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserInvalidTest, MissingCommaInArray)
{
    Parser parser({Token(Token::LBRACKET),
                   Token(Token::INTEGER, "1"),
                   Token(Token::INTEGER, "2"),
                   Token(Token::RBRACKET)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserInvalidTest, ExtraCommaInArray)
{
    Parser parser({Token(Token::LBRACKET),
                   Token(Token::INTEGER, "1"),
                   Token(Token::COMMA),
                   Token(Token::RBRACKET)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserInvalidTest, ExtraCommaInObject)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::STRING, "key"),
                   Token(Token::COLON),
                   Token(Token::INTEGER, "42"),
                   Token(Token::COMMA),
                   Token(Token::RBRACE)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserInvalidTest, ConsecutiveValuesWithoutComma)
{
    Parser parser({Token(Token::LBRACKET),
                   Token(Token::INTEGER, "1"),
                   Token(Token::INTEGER, "2"),
                   Token(Token::RBRACKET)});
    EXPECT_FALSE(parser.validate());
}

TEST(ParserInvalidTest, InvalidTokenAtTopLevel)
{
    Parser parser({Token(Token::STRING, "hello")});
    EXPECT_FALSE(parser.validate());

    Parser parser2({Token(Token::TRUE)});
    EXPECT_FALSE(parser2.validate());
}

TEST(ParserInvalidTest, NestedUnmatchedBrackets)
{
    Parser parser({Token(Token::LBRACKET),
                   Token(Token::LBRACE),
                   Token(Token::STRING, "key"),
                   Token(Token::COLON),
                   Token(Token::INTEGER, "1"),
                   Token(Token::RBRACKET), // closing wrong type
                   Token(Token::RBRACE)});
    EXPECT_FALSE(parser.validate());
}