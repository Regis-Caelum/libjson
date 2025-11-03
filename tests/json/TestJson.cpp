#include "json/Json.h"

#include <gtest/gtest.h>

// ----------------------------
// JsonValue type tests
// ----------------------------

TEST(JsonValueTest, NullValue)
{
	JsonValue j(nullptr);
	EXPECT_TRUE(j.is_null());
	EXPECT_FALSE(j.is_boolean());
	EXPECT_FALSE(j.is_integer());
	EXPECT_FALSE(j.is_string());
}

TEST(JsonValueTest, BooleanValue)
{
	JsonValue j(true);
	EXPECT_TRUE(j.is_boolean());
	EXPECT_EQ(static_cast<bool>(j), true);

	j = false;
	EXPECT_TRUE(j.is_boolean());
	EXPECT_EQ(static_cast<bool>(j), false);
}

TEST(JsonValueTest, IntegerValue)
{
	JsonValue j(42);
	EXPECT_TRUE(j.is_integer());
	EXPECT_EQ(static_cast<JsonValue::number_integer_t>(j), 42);

	j = int64_t(-100);
	EXPECT_TRUE(j.is_integer());
	EXPECT_EQ(static_cast<JsonValue::number_integer_t>(j), -100);
}

TEST(JsonValueTest, FloatValue)
{
	JsonValue j(3.14);
	EXPECT_TRUE(j.is_float());
	EXPECT_DOUBLE_EQ(static_cast<JsonValue::number_float_t>(j), 3.14);
}

TEST(JsonValueTest, StringValue)
{
	JsonValue j("hello");
	EXPECT_TRUE(j.is_string());
	EXPECT_EQ(static_cast<JsonValue::string_t>(j), "hello");

	j = std::string("world");
	EXPECT_TRUE(j.is_string());
	EXPECT_EQ(static_cast<JsonValue::string_t>(j), "world");
}

TEST(JsonValueTest, ArrayValue)
{
	JsonValue j({1, 2, 3});
	EXPECT_TRUE(j.is_array());

	auto arr = static_cast<JsonValue::array_t>(j);
	ASSERT_EQ(arr.size(), 3);
	EXPECT_EQ(static_cast<JsonValue::number_integer_t>(arr[0]), 1);
	EXPECT_EQ(static_cast<JsonValue::number_integer_t>(arr[2]), 3);
}

TEST(JsonValueTest, ObjectValue)
{
	JsonValue::object_t obj;
	obj["key"] = 123;
	JsonValue j(obj);

	EXPECT_TRUE(j.is_object());
	auto o = static_cast<JsonValue::object_t>(j);
	EXPECT_EQ(static_cast<JsonValue::number_integer_t>(o["key"]), 123);
}

// ----------------------------
// Json class tests
// ----------------------------

TEST(JsonTest, EmptyJson)
{
	Json j;
	EXPECT_TRUE(j.empty());
}

TEST(JsonTest, AddAndAccessObject)
{
	Json j;
	j["name"] = "Alice";
	j["age"] = 30;

	EXPECT_FALSE(j.empty());
	EXPECT_EQ(static_cast<JsonValue::string_t>(j["name"]), "Alice");
	EXPECT_EQ(static_cast<JsonValue::number_integer_t>(j["age"]), 30);
}

TEST(JsonTest, IteratorAccess)
{
	Json j;
	j["x"] = 10;
	j["y"] = 20;

	std::vector<std::string> keys;
	for (const auto &kv : j)
	{
		keys.push_back(kv.first);
	}

	EXPECT_EQ(keys.size(), 2);
	EXPECT_NE(std::find(keys.begin(), keys.end(), "x"), keys.end());
	EXPECT_NE(std::find(keys.begin(), keys.end(), "y"), keys.end());
}

// ----------------------------
// Operator<< tests
// ----------------------------

TEST(JsonValueTest, StreamOutput)
{
	JsonValue j1(nullptr);
	std::ostringstream os1;
	os1 << j1;
	EXPECT_EQ(os1.str(), "null");

	JsonValue j2(42);
	std::ostringstream os2;
	os2 << j2;
	EXPECT_EQ(os2.str(), "42");

	JsonValue j3("hello");
	std::ostringstream os3;
	os3 << j3;
	EXPECT_EQ(os3.str(), "\"hello\"");

	JsonValue j4({1, 2, 3});
	std::ostringstream os4;
	os4 << j4;
	EXPECT_EQ(os4.str(), "[1, 2, 3]");

	JsonValue::object_t obj;
	obj["a"] = 1;
	obj["b"] = "hi";
	JsonValue j5(obj);
	std::ostringstream os5;
	os5 << j5;
	std::string out = os5.str();
	EXPECT_NE(out.find("\"a\""), std::string::npos);
	EXPECT_NE(out.find("\"b\""), std::string::npos);
}
