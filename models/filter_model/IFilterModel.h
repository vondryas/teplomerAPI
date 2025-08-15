#pragma once

#include <models/model_interface/IUserModel.h>

namespace filter_model
{
	struct IFilterModel : public model_interface::IUserModel
	{
		virtual ~IFilterModel() = default;
		// Method to validate the filter criteria

		void mapToOrmModel(model_interface::IModel& model) const override {}
		void mapToListModel(model_interface::IModel& model) const override {}
		void mapToRequestModel(model_interface::IModel& model) const override {}
	};
}