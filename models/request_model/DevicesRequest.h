#pragma once

#include "IRequestModel.h"

#include <optional>

namespace drogon_model::defaultdb
{
	class Devices;
}

namespace request_model
{
	struct DevicesRequest : IRequestModel
	{
		using Devices = drogon_model::defaultdb::Devices;

		std::string name;
		std::optional<int32_t> type;
		std::optional<uuids::uuid> userId; // Assuming userId is a UUID


		DevicesRequest() = default;
		inline bool isEmpty() const
		{
			return !id.has_value() && name.empty() && !type.has_value() && !userId.has_value();
		}
		void mapToOrmModel(model_interface::IModel& ormModel) const override;
		void mapToListModel(model_interface::IModel& ormModel) const override
		{
			throw std::runtime_error("Mapping to list model is not implemented for Users request model");
		}
		void mapToRequestModel(model_interface::IModel& ormModel) const override
		{
			LOG_INFO << "Mapping ORM model to request model";
			throw std::runtime_error("Mapping to request model is not implemented for Users ORM model");
		}
		Json::Value toJson() const override
		{
			Json::Value json;
			if (!name.empty())
				json["name"] = name;
			if (type.has_value())
				json["type"] = *type;
			if (userId.has_value())
				json["userId"] = uuids::to_string(*userId);
			return json;
		}

		std::pair<bool, std::string> validateForCreate() const override
		{
			LOG_INFO << "Validating request for create operation";
			bool result = true;
			std::string errorMessage;
			if (userId.has_value())
			{
				//result = result && !userId->is_nil();
				errorMessage += result ? "" : "Invalid UUID format for userId\n";
			}
			else
			{
				result = false;
				errorMessage += "userId must be specified\n";
			}
			if (name.empty())
			{
				result = false;
				errorMessage += "name cannot be empty\n";
			}
			if (!type.has_value())
			{
				result = false;
				errorMessage += "type must be specified\n";
			}
			return { result, errorMessage };
		}

		std::pair<bool, std::string> validateForUpdate() const
		{
			LOG_INFO << "Validating request for update operation";
			if (name.empty())
			{
				return { false, "Name identificator must be set for update operation" };
			}
			return { true, "" };
		}
	};
}

namespace drogon
{
	template <>
	inline request_model::DevicesRequest fromRequest<request_model::DevicesRequest>(const HttpRequest& req)
	{
		LOG_INFO << "parse request";
		auto json = req.getJsonObject();
		request_model::DevicesRequest data;
		Json::Value& jsonRef = *json;

		if (json)
		{
			LOG_INFO << "Received JSON: " << json->toStyledString();
			data.name = jsonRef["name"].isNull() ? "" : jsonRef["name"].asString();
			data.type = jsonRef["type"].isNull() ? std::nullopt : std::make_optional(jsonRef["type"].asInt());
			data.userId = jsonRef["userId"].isNull() ? std::nullopt : std::make_optional(uuids::uuid::from_string(jsonRef["userId"].asString()).value());
		}

		return data;
	}
}