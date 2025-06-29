#include <nlohmann/json.hpp>
#include <stdexcept>


namespace json_util
{
	template <typename T> bool set_if_exists(T &dest, const nlohmann::json &data, const std::string &key)
	{
		if (!data.contains(key))
		{
			return false; // Key does not exist, do not set the value
		}

		dest = data[key].get<T>();
		return true;
	}

	template <typename T> void set_mandatory(T &dest, const nlohmann::json &data, const std::string &key)
	{
		if (!data.contains(key) || data[key].is_null())
		{
			throw std::runtime_error("'" + key + "' must be specified");
		}

		dest = data[key].get<T>();
	}
} // namespace json_util
