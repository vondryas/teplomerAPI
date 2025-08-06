#pragma once

#include <models/orm_model/Devices.h>
#include "IRequestModel.h"
#include <optional>

namespace drogon_model::defaultdb
{
	class Devices;
}

namespace request_model
{
	struct UsersRequest : IRequestModel
	{
		using Users = drogon_model::defaultdb::Users;

		std::string username;
		std::string email;
		std::string password;
		std::optional<int32_t> role = 0;
		std::optional<bool> remember = false;

		UsersRequest() = default;
		inline bool isEmpty() const
		{
			return !id.has_value() && username.empty() && email.empty() 
				&& password.empty() && !role.has_value() && !remember.has_value();
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
			if (!username.empty())
				json["username"] = username;
			if (!email.empty())
				json["email"] = email;
			if (!password.empty())
				json["password"] = password;
			if (role.has_value())
				json["role"] = *role;
			if (remember.has_value())
				json["remember"] = *remember;
			return json;
		}

		std::pair<bool, std::string> validateForCreate() const override
		{
			bool result = true;
			std::string errorMessage;
			if (id.has_value())
			{
				result = result && uuids::uuid::is_valid_uuid(uuids::to_string(*id));
				errorMessage += result ? "" : "Invalid UUID format for ID\n";
			}
			if (username.empty())
			{
				result = false;
				errorMessage += "Username cannot be empty\n";
			}
			if (email.empty())
			{
				result = false;
				errorMessage += "Email cannot be empty\n";
			}
			if (password.empty())
			{
				result = false;
				errorMessage += "Password cannot be empty\n";
			}
			return { result, errorMessage };
		}
	};
}