#pragma once 


#include "IRequestModel.h"

namespace drogon_model::defaultdb
{
	class WeatherStationData;
}


namespace request_model
{
	struct WeatherStationDataRequest : IRequestModel
	{
		using WeatherStationData = drogon_model::defaultdb::WeatherStationData;

		std::optional<int32_t> battery;
		std::optional<double> temperature;
		std::optional<double> humidity;
		std::optional<double> pressure;
		std::string deviceId;
		std::string date;
		std::string time;
		WeatherStationDataRequest() = default;

		void mapToOrmModel(model_interface::IModel& ormModel) const override;


		void mapToListModel(model_interface::IModel& ormModel) const override
		{
			throw std::runtime_error("Mapping to list model is not implemented for SensorData request model");
		}

		void mapToRequestModel(model_interface::IModel& ormModel) const override
		{
			LOG_INFO << "Mapping ORM model to request model";
			throw std::runtime_error("Mapping to request model is not implemented for SensorData ORM model");
		}

		inline bool isEmpty() const
		{
			return !id.has_value() && !battery.has_value() &&
				!temperature.has_value() && !humidity.has_value() && !pressure.has_value() &&
				deviceId.empty() && date.empty() && time.empty();
		}

		Json::Value toJson() const override
		{
			Json::Value json;
			if (id.has_value())
				json["id"] = uuids::to_string(*id);
			if (battery.has_value())
				json["battery"] = *battery;
			if (temperature.has_value())
				json["temperature"] = *temperature;
			if (humidity.has_value())
				json["humidity"] = *humidity;
			if (pressure.has_value())
				json["pressure"] = *pressure;
			if (!deviceId.empty())
				json["deviceId"] = deviceId;
			if (!date.empty())
				json["date"] = date;
			if (!time.empty())
				json["time"] = time;
			return json;
		}

		std::pair<bool, std::string> validateForCreate() const override
		{
			LOG_INFO << "Validating request for create operation";
			bool result = true;
			std::string errorMessage;
			if (id.has_value())
			{
				result = result && uuids::uuid::is_valid_uuid(uuids::to_string(*id));
				errorMessage += result ? "" : "Invalid UUID format for ID\n";
			}
			if (deviceId.empty())
			{
				result = false;
				errorMessage += "Device ID must be provided\n";
			}
			if (!battery.has_value() || !temperature.has_value() || !humidity.has_value() || !pressure.has_value())
			{
				result = false;
				errorMessage += "All sensor data fields must be provided\n";
			}
			if (date.empty() || time.empty())
			{
				result = false;
				errorMessage += "Date and time must be provided\n";
			}
			return { result, errorMessage };
		}

	public:
		void correctDate();
	};
}

namespace drogon
{
	template <>
	inline request_model::WeatherStationDataRequest fromRequest<request_model::WeatherStationDataRequest>(const HttpRequest& req)
	{
		LOG_INFO << "parse request";
		auto json = req.getJsonObject();
		request_model::WeatherStationDataRequest data;
		LOG_INFO << "Received JSON: " << json->toStyledString();
		if (json)
		{
			if (!(*json)["uplink_message"]["decoded_payload"].isNull())
			{
				data.pressure = (*json)["uplink_message"]["decoded_payload"]["pressure"].isNull() ? std::nullopt : std::make_optional((*json)["uplink_message"]["decoded_payload"]["pressure"].asDouble());
				data.temperature = (*json)["uplink_message"]["decoded_payload"]["temperature"].isNull() ? std::nullopt : std::make_optional((*json)["uplink_message"]["decoded_payload"]["temperature"].asDouble());
				data.humidity = (*json)["uplink_message"]["decoded_payload"]["humidity"].isNull() ? std::nullopt : std::make_optional((*json)["uplink_message"]["decoded_payload"]["humidity"].asDouble());
				data.battery = (*json)["uplink_message"]["decoded_payload"]["battery"].isNull() ? std::nullopt : std::make_optional((*json)["uplink_message"]["decoded_payload"]["battery"].asInt());
				data.date = (*json)["uplink_message"]["decoded_payload"]["date"].isNull() ? "" : (*json)["uplink_message"]["decoded_payload"]["date"].asString();
				data.time = (*json)["uplink_message"]["decoded_payload"]["time"].isNull() ? "" : (*json)["uplink_message"]["decoded_payload"]["time"].asString();
				data.id = (*json)["uplink_message"]["decoded_payload"]["id"].isNull() ? std::nullopt : std::make_optional(uuids::uuid::from_string((*json)["uplink_message"]["decoded_payload"]["id"].asString()).value());
			}
			else
			{
				data.pressure = (*json)["pressure"].isNull() ? std::nullopt : std::make_optional((*json)["pressure"].asDouble());
				data.temperature = (*json)["temperature"].isNull() ? std::nullopt : std::make_optional((*json)["temperature"].asDouble());
				data.humidity = (*json)["humidity"].isNull() ? std::nullopt : std::make_optional((*json)["humidity"].asDouble());
				data.battery = (*json)["battery"].isNull() ? std::nullopt : std::make_optional((*json)["battery"].asInt());
				data.date = (*json)["date"].isNull() ? "" : (*json)["date"].asString();
				data.time = (*json)["time"].isNull() ? "" : (*json)["time"].asString();
				data.id = (*json)["id"].isNull() ? std::nullopt : std::make_optional(uuids::uuid::from_string((*json)["id"].asString()).value());
			}
			if (!(*json)["end_device_ids"]["device_id"].isNull())
			{
				data.deviceId = (*json)["end_device_ids"]["device_id"].isNull() ? "" : (*json)["end_device_ids"]["device_id"].asString();
			}
			else
			{
				data.deviceId = (*json)["deviceId"].isNull() ? "" : (*json)["deviceId"].asString();
			}
		}
		data.correctDate();
		return data;
	}
}