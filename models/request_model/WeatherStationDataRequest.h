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
	};
}