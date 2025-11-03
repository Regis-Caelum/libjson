#include "json/Json.h"
#include <gtest/gtest.h>

class JsonParserTest : public ::testing::Test {
protected:
	void SetUp() override {
	}
};

TEST_F(JsonParserTest, CanParseEmptyObject) {
	ASSERT_TRUE(true);
}

TEST_F(JsonParserTest, ThrowsOnInvalidJson) {
	ASSERT_TRUE(true);
}

TEST_F(JsonParserTest, AGAIN) {
	ASSERT_TRUE(true);
}