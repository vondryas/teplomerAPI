#pragma once

#include <models/model_mapper/IModelMapper.h>

namespace model_interface
{
	class IModel : public model_mapper::IModelMapper
	{
	public:
		virtual ~IModel() = default;

		virtual Json::Value toJson() const = 0;
	};
} 