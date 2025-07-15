#pragma once

#include "IModel.h"

namespace model_interface
{

	class IOrmModel : public IModel {
	public:
		virtual ~IOrmModel() = default;
	};
}
