#pragma once

#include "IRequestModel.h"
#include <optional>

namespace request_model
{
	struct DevicesRequest : IRequestModel
	{
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
			if (id.has_value())
				json["id"] = uuids::to_string(*id);
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
			if (id.has_value())
			{
				result = result && uuids::uuid::is_valid_uuid(uuids::to_string(*id));
				errorMessage += result ? "" : "Invalid UUID format for ID\n";
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
			if (!userId.has_value())
			{
				result = false;
				errorMessage += "userId must be specified\n";
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