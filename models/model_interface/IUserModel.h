#pragma once 

#include "IModel.h"
#include <json/json.h>

namespace model_interface
{

	class IUserModel : public IModel
	{
	public:
		virtual ~IUserModel() = default;
	};
}