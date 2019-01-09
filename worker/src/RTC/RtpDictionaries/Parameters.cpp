#define MS_CLASS "RTC::Parameters"
// #define MS_LOG_DEV

#include "RTC/Parameters.hpp"
#include "Logger.hpp"

namespace RTC
{
	/* Instance methods. */

	void Parameters::FillJson(json& jsonObject) const
	{
		MS_TRACE();

		for (auto& kv : this->mapKeyValues)
		{
			auto& key   = kv.first;
			auto& value = kv.second;

			switch (value.type)
			{
				case Value::Type::BOOLEAN:
					jsonObject[key] = value.booleanValue;
					break;

				case Value::Type::INTEGER:
					jsonObject[key] = value.integerValue;
					break;

				case Value::Type::DOUBLE:
					jsonObject[key] = value.doubleValue;
					break;

				case Value::Type::STRING:
					jsonObject[key] = value.stringValue;
					break;

				case Value::Type::ARRAY_OF_INTEGERS:
					jsonObject[key] = value.arrayOfIntegers;
					break;
			}
		}
	}

	void Parameters::Set(json& data)
	{
		MS_TRACE();

		MS_ASSERT(data.is_object(), "data is not an object");

		for (json::iterator it = data.begin(); it != data.end(); ++it)
		{
			std::string key = it.key();
			auto& value     = it.value();

			switch (value.type())
			{
				case json::value_t::boolean:
				{
					this->mapKeyValues[key] = Value(value.get<bool>());

					break;
				}

				case json::value_t::number_integer:
				{
					this->mapKeyValues[key] = Value(value.get<int32_t>());

					break;
				}

				case json::value_t::number_float:
				{
					this->mapKeyValues[key] = Value(value.get<double>());

					break;
				}

				case json::value_t::string:
				{
					this->mapKeyValues[key] = Value(value.get<std::string>());

					break;
				}

				case json::value_t::array:
				{
					std::vector<int32_t> arrayOfIntegers;
					bool isValid = true;

					for (auto& entry : value)
					{
						if (!entry.is_number_integer())
						{
							isValid = false;

							break;
						}

						arrayOfIntegers.push_back(entry.get<int32_t>());
					}

					if (!arrayOfIntegers.empty() && isValid)
						this->mapKeyValues[key] = Value(arrayOfIntegers);

					break;
				}

				default:; // Just ignore other value types.
			}
		}
	}

	bool Parameters::GetBoolean(const std::string& key)
	{
		MS_TRACE();

		MS_ASSERT(
		  this->mapKeyValues.find(key) != this->mapKeyValues.end(),
		  "key does not exist [key:%s]",
		  key.c_str());

		return this->mapKeyValues[key].booleanValue;
	}

	int32_t Parameters::GetInteger(const std::string& key)
	{
		MS_TRACE();

		MS_ASSERT(
		  this->mapKeyValues.find(key) != this->mapKeyValues.end(),
		  "key does not exist [key:%s]",
		  key.c_str());

		return this->mapKeyValues[key].integerValue;
	}

	double Parameters::GetDouble(const std::string& key)
	{
		MS_TRACE();

		MS_ASSERT(
		  this->mapKeyValues.find(key) != this->mapKeyValues.end(),
		  "key does not exist [key:%s]",
		  key.c_str());

		return this->mapKeyValues[key].doubleValue;
	}

	std::string& Parameters::GetString(const std::string& key)
	{
		MS_TRACE();

		MS_ASSERT(
		  this->mapKeyValues.find(key) != this->mapKeyValues.end(),
		  "key does not exist [key:%s]",
		  key.c_str());

		return this->mapKeyValues[key].stringValue;
	}

	std::vector<int32_t>& Parameters::GetArrayOfIntegers(const std::string& key)
	{
		MS_TRACE();

		MS_ASSERT(
		  this->mapKeyValues.find(key) != this->mapKeyValues.end(),
		  "key does not exist [key:%s]",
		  key.c_str());

		return this->mapKeyValues[key].arrayOfIntegers;
	}
} // namespace RTC
