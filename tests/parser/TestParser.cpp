#include "parser/Parser.h"
#include <gtest/gtest.h>

class ParserTest : public ::testing::Test
{
};

// ------------------- Valid JSON -------------------

TEST_F(ParserTest, ValidEmptyObject)
{
    Parser parser({Token(Token::LBRACE), Token(Token::RBRACE)});
    EXPECT_TRUE(parser.validate());
}

TEST_F(ParserTest, ValidEmptyArray)
{
    Parser parser({Token(Token::LBRACKET), Token(Token::RBRACKET)});
    EXPECT_TRUE(parser.validate());
}

TEST_F(ParserTest, ValidNested)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::STRING, "arr"), Token(Token::COLON),
                   Token(Token::LBRACKET),
                   Token(Token::RBRACKET),
                   Token(Token::RBRACE)});
    EXPECT_TRUE(parser.validate());
}

// ------------------- Invalid JSON -------------------

TEST_F(ParserTest, InvalidMissingClosingBrace)
{
    Parser parser({Token(Token::LBRACE)});
    EXPECT_FALSE(parser.validate());
}

TEST_F(ParserTest, InvalidMissingClosingBracket)
{
    Parser parser({Token(Token::LBRACKET)});
    EXPECT_FALSE(parser.validate());
}

TEST_F(ParserTest, InvalidExtraCommaInArray)
{
    Parser parser({
        Token(Token::LBRACKET),
        Token(Token::INTEGER, "1"),
        Token(Token::COMMA),
        Token(Token::RBRACKET), // [1,]
    });
    EXPECT_FALSE(parser.validate());
}

TEST_F(ParserTest, InvalidExtraCommaInObject)
{
    Parser parser({
        Token(Token::LBRACE),
        Token(Token::STRING, "key"),
        Token(Token::COLON),
        Token(Token::INTEGER, "1"),
        Token(Token::COMMA),
        Token(Token::RBRACE), // {"key":1,}
    });
    EXPECT_FALSE(parser.validate());
}

TEST_F(ParserTest, InvalidConsecutiveValuesWithoutComma)
{
    Parser parser({Token(Token::LBRACKET),
                   Token(Token::INTEGER, "1"),
                   Token(Token::INTEGER, "2"), // missing comma
                   Token(Token::RBRACKET)});
    EXPECT_FALSE(parser.validate());
}

TEST_F(ParserTest, InvalidMissingColonInObject)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::STRING, "key"),
                   Token(Token::INTEGER, "1"), // missing colon
                   Token(Token::RBRACE)});
    EXPECT_FALSE(parser.validate());
}

TEST_F(ParserTest, InvalidTokenAtTopLevel)
{
    Parser parser({Token(Token::STRING, "abc")}); // invalid standalone value
    EXPECT_FALSE(parser.validate());
}

// ------------------- Build JSON -------------------

TEST_F(ParserTest, BuildSimpleObject)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::STRING, "a"), Token(Token::COLON), Token(Token::INTEGER, "10"),
                   Token(Token::RBRACE)});

    Json result = parser.buildJson();
    EXPECT_EQ(static_cast<JsonValue::number_integer_t>(result["a"]), 10);
}

TEST_F(ParserTest, BuildNestedArray)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::STRING, "nums"), Token(Token::COLON),
                   Token(Token::LBRACKET),
                   Token(Token::INTEGER, "1"),
                   Token(Token::COMMA),
                   Token(Token::INTEGER, "2"),
                   Token(Token::RBRACKET),
                   Token(Token::RBRACE)});

    Json result = parser.buildJson();
    const auto &arr = static_cast<JsonValue::array_t>(result["nums"]);
    ASSERT_EQ(arr.size(), 2);
    EXPECT_EQ(static_cast<JsonValue::number_integer_t>(arr[0]), 1);
    EXPECT_EQ(static_cast<JsonValue::number_integer_t>(arr[1]), 2);
}

TEST_F(ParserTest, BuildBooleanAndNull)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::STRING, "ok"), Token(Token::COLON), Token(Token::TRUE),
                   Token(Token::COMMA),
                   Token(Token::STRING, "none"), Token(Token::COLON), Token(Token::NULLTOKEN),
                   Token(Token::RBRACE)});

    Json result = parser.buildJson();
    EXPECT_TRUE(static_cast<bool>(result["ok"]));
    EXPECT_TRUE(result["none"].is_null());
}

TEST_F(ParserTest, BuildInvalidThrows)
{
    Parser parser({Token(Token::LBRACE),
                   Token(Token::STRING, "key"),
                   Token(Token::INTEGER, "1"), // missing colon
                   Token(Token::RBRACE)});

    EXPECT_THROW(parser.buildJson(), std::runtime_error);
}

TEST_F(ParserTest, BuildNonObjectRootThrows)
{
    Parser parser({Token(Token::LBRACKET),
                   Token(Token::INTEGER, "1"),
                   Token(Token::RBRACKET)});

    EXPECT_THROW(parser.buildJson(), std::runtime_error);
}
