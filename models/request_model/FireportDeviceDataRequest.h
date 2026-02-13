#pragma once
#include "IRequestModel.h"

namespace drogon_model::defaultdb
{
	class FireportDeviceData;
}

namespace request_model
{
	struct FireportDeviceDataRequest : IRequestModel
	{
		using FireportDeviceData = drogon_model::defaultdb::FireportDeviceData;
		std::string deviceId;
		std::string password;
		FireportDeviceDataRequest() = default;
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
			return !id.has_value() && deviceId.empty() && password.empty();
		}

		Json::Value toJson() const override
		{
			Json::Value json;
			if (id.has_value())
				json["id"] = uuids::to_string(*id);
			if (!deviceId.empty())
				json["deviceId"] = deviceId;
			if (!password.empty())
				json["password"] = password;
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
			if (password.empty())
			{
				result = false;
				errorMessage += "Device address must be provided\n";
			}
			return { result, errorMessage };
		}

		std::pair<bool, std::string> validateForUpdate() const
		{
			LOG_INFO << "Validating request for update operation";
			bool result = true;
			std::string errorMessage;
			if (!id.has_value())
			{
				result = false;
				errorMessage += "ID must be provided for update operation\n";
			}
			if (!deviceId.empty())
			{
				result = false;
				errorMessage += "Device ID cannot be provided for update\n";
			}
			if (password.empty())
			{
				result = false;
				errorMessage += "Device address must be provided\n";
			}
			return { result, errorMessage };
		}
	};
}

namespace drogon
{
	template<>
	inline request_model::FireportDeviceDataRequest fromRequest<request_model::FireportDeviceDataRequest>(const HttpRequest& req)
	{
		LOG_INFO << "parse request";
		auto json = req.getJsonObject();
		request_model::FireportDeviceDataRequest data;
		LOG_INFO << "Received JSON: " << json->toStyledString();
		if (json)
		{
			data.deviceId = (*json)["deviceId"].isNull() ? "" : (*json)["deviceId"].asString();
			data.password = (*json)["password"].isNull() ? "" : (*json)["password"].asString();
			data.id = (*json)["id"].isNull() ? std::nullopt : std::make_optional(uuids::uuid::from_string((*json)["id"].asString()).value());
		}
		return data;
	}
}