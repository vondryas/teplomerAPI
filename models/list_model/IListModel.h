#pragma once

#include <models/model_interface/IUserModel.h>

namespace list_model
{

	class IListModel : public model_interface::IUserModel
	{
	public:
		virtual ~IListModel() = default;


	};
}
