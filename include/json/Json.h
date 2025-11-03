#ifndef JSON_H
#define JSON_H

class JsonValue;

class Json
{
public:
	Json() = default;
	Json(const Json &) = default;

	JsonValue &operator[](const std::string &key) { return object_[key]; }
	JsonValue &operator[](const char *key) { return object_[std::string(key)]; }

	bool empty() const { return object_.empty(); }

	auto begin() { return object_.begin(); }
	auto end() { return object_.end(); }

	auto begin() const { return object_.begin(); }
	auto end() const { return object_.end(); }

private:
	std::unordered_map<std::string, JsonValue> object_;
};

#endif // !JSON_H
