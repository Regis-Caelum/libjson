#ifndef JSON_H
#define JSON_H

#include <variant>
#include <vector>
#include <string>
#include <unordered_map>
#include <type_traits>
#include <initializer_list>
#include <cstdint>
#include <iostream>

class JsonValue;

class Json
{
public:
    using object_t = std::unordered_map<std::string, JsonValue>;

    Json() = default;
    Json(const Json &) = default;
    Json(Json &&) noexcept = default;
    Json &operator=(const Json &) = default;
    Json &operator=(Json &&) noexcept = default;

    Json(std::initializer_list<std::pair<const std::string, JsonValue>> list)
        : object_(list) {}

    JsonValue &operator[](const std::string &key) { return object_[key]; }
    JsonValue &operator[](const char *key) { return object_[std::string(key)]; }

    bool empty() const { return object_.empty(); }

    auto begin() { return object_.begin(); }
    auto end() { return object_.end(); }
    auto begin() const { return object_.begin(); }
    auto end() const { return object_.end(); }

private:
    object_t object_;
};

class JsonValue
{
public:
    using object_t = Json;
    using array_t = std::vector<JsonValue>;
    using string_t = std::string;
    using boolean_t = bool;
    using number_integer_t = int64_t;
    using number_float_t = double;
    using nullptr_t = std::nullptr_t;

    using value_t = std::variant<
        object_t,
        array_t,
        string_t,
        boolean_t,
        number_integer_t,
        number_float_t,
        nullptr_t>;

    JsonValue() = default;
    JsonValue(const JsonValue &) = default;
    JsonValue(JsonValue &&) noexcept = default;
    JsonValue &operator=(const JsonValue &) = default;
    JsonValue &operator=(JsonValue &&) noexcept = default;

    template <typename T>
    JsonValue(T &&val) : value_(convert(std::forward<T>(val))) {}

    template <typename T>
    JsonValue &operator=(T &&val)
    {
        value_ = convert(std::forward<T>(val));
        return *this;
    }

    JsonValue(std::initializer_list<JsonValue> list) : value_(array_t(list)) {}

    explicit operator string_t() const { return std::get<string_t>(value_); }
    explicit operator object_t() const { return std::get<object_t>(value_); }
    explicit operator array_t() const { return std::get<array_t>(value_); }
    explicit operator boolean_t() const { return std::get<boolean_t>(value_); }
    explicit operator number_integer_t() const { return std::get<number_integer_t>(value_); }
    explicit operator number_float_t() const { return std::get<number_float_t>(value_); }

    bool is_null() const { return std::holds_alternative<nullptr_t>(value_); }
    bool is_boolean() const { return std::holds_alternative<boolean_t>(value_); }
    bool is_integer() const { return std::holds_alternative<number_integer_t>(value_); }
    bool is_float() const { return std::holds_alternative<number_float_t>(value_); }
    bool is_string() const { return std::holds_alternative<string_t>(value_); }
    bool is_array() const { return std::holds_alternative<array_t>(value_); }
    bool is_object() const { return std::holds_alternative<object_t>(value_); }

private:
    value_t value_;

    template <typename T>
    static value_t convert(T &&val)
    {
        using DecayT = std::decay_t<T>;
        if constexpr (std::is_same_v<DecayT, nullptr_t>)
            return nullptr;
        else if constexpr (std::is_same_v<DecayT, const char *>)
            return string_t(val);
        else if constexpr (std::is_same_v<DecayT, string_t>)
            return val;
        else if constexpr (std::is_integral_v<DecayT> && !std::is_same_v<DecayT, bool>)
            return static_cast<number_integer_t>(val);
        else if constexpr (std::is_same_v<DecayT, boolean_t>)
            return val;
        else if constexpr (std::is_floating_point_v<DecayT>)
            return static_cast<number_float_t>(val);
        else if constexpr (std::is_same_v<DecayT, array_t>)
            return val;
        else if constexpr (std::is_same_v<DecayT, object_t>)
            return val;
        else if constexpr (std::is_same_v<DecayT, JsonValue>)
            return val.value_;
        else
            static_assert(always_false<DecayT>, "Unsupported type for JsonValue");
    }

    template <typename>
    inline static constexpr bool always_false = false;
};

inline std::ostream &operator<<(std::ostream &os, const JsonValue &json);

inline std::ostream &operator<<(std::ostream &os, const Json &j)
{
    os << '{';
    bool first = true;
    for (const auto &kv : j)
    {
        if (!first)
            os << ", ";
        os << '\"' << kv.first << "\": " << kv.second;
        first = false;
    }
    os << '}';
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const JsonValue &json)
{
    if (json.is_null())
    {
        os << "null";
    }
    else if (json.is_boolean())
    {
        os << std::boolalpha << static_cast<bool>(json);
    }
    else if (json.is_integer())
    {
        os << static_cast<JsonValue::number_integer_t>(json);
    }
    else if (json.is_float())
    {
        os << static_cast<JsonValue::number_float_t>(json);
    }
    else if (json.is_string())
    {
        os << '\"' << static_cast<JsonValue::string_t>(json) << '\"';
    }
    else if (json.is_array())
    {
        os << '[';
        const auto &arr = static_cast<JsonValue::array_t>(json);
        for (size_t i = 0; i < arr.size(); ++i)
        {
            if (i > 0)
                os << ", ";
            os << arr[i];
        }
        os << ']';
    }
    else if (json.is_object())
    {
        os << static_cast<JsonValue::object_t>(json);
    }
    return os;
}

#endif // !JSON_H
