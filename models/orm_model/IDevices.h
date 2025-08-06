#pragma once

#include <models/model_interface/IOrmModel.h>

namespace drogon_model::defaultdb
{
	class IDevices : public model_interface::IOrmModel
	{
	public:
		virtual ~IDevices() = default;

		virtual void setName(const std::string& pName) noexcept = 0;
		virtual void setName(std::string&& pName) noexcept = 0;
		virtual void setUserId(const std::string& pUserId) noexcept = 0;
		virtual void setType(const int32_t& pType) noexcept = 0;
		virtual const std::shared_ptr<std::string>& getName() const noexcept = 0;
		virtual const std::shared_ptr<std::string>& getUserId() const noexcept = 0;
		virtual const std::shared_ptr<int32_t>& getType() const noexcept = 0;

		void mapToOrmModel(model_interface::IModel& model) const override
		{
			throw std::runtime_error("Mapping to ORM model is not implemented for user ORM model");
		}
		void mapToListModel(model_interface::IModel& model) const override
		{
			throw std::runtime_error("Mapping to list model is not implemented for user ORM model");
		}
		void mapToRequestModel(model_interface::IModel& model) const override
		{
			throw std::runtime_error("Mapping to request model is not implemented for User ORM model");
		}
	};
}