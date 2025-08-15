#pragma once

#include <models/model_interface/IUserModel.h> 
#include <stduuid/uuid.h>

namespace auth_model
{
	class AuthModel : public model_interface::IUserModel
	{
	public:
		std::string username;
		std::string token;
		std::string email;
		uuids::uuid userId;
		AuthModel() = default;
		void mapToOrmModel(model_interface::IModel& model) const override {}
		void mapToListModel(model_interface::IModel& model) const override {}
		void mapToRequestModel(model_interface::IModel& model) const override {}
		Json::Value toJson() const override {
			Json::Value json;
			json["userId"] = uuids::to_string(userId);
			json["username"] = username;
			json["token"] = token;
			json["email"] = email;
			return json;
		}
	};
}