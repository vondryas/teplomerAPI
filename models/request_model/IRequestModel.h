#pragma once

#include <stduuid/uuid.h>
#include <models/orm_model/SensorData.h>
#include <models/model_interface/IUserModel.h>
#include <utility>

namespace request_model
{
	struct IRequestModel : public model_interface::IUserModel
	{
		std::optional<uuids::uuid> id;
		virtual ~IRequestModel() = default;

		virtual std::pair<bool, std::string> validateForCreate() const = 0;

		std::pair<bool, std::string> validateForUpdate() const
		{
			if (!id.has_value())
			{
				return { false, "ID must be set for update operation" };
			}
			return { true, "" };
		}
	};
}