#pragma once

#include <models/model_interface/IOrmModel.h>

namespace drogon_model::defaultdb
{
	class IUsers : public model_interface::IOrmModel
	{
	public:
		virtual ~IUsers() = default;

		virtual void setId(const std::string& pId) noexcept = 0;
		virtual void setId(std::string&& pId) noexcept = 0;
		virtual void setRole(const int32_t& pRole) noexcept = 0;
		virtual void setPasshash(const std::string& pPasshash) noexcept = 0;
		virtual void setUsername(const std::string& pUsername) noexcept = 0;
		virtual void setEmail(const std::string& pEmail) noexcept = 0;
		virtual const std::shared_ptr<std::string>& getId() const noexcept = 0;
		virtual const std::shared_ptr<int32_t>& getRole() const noexcept = 0;
		virtual const std::shared_ptr<std::string>& getPasshash() const noexcept = 0;
		virtual const std::shared_ptr<std::string>& getUsername() const noexcept = 0;
		virtual const std::shared_ptr<std::string>& getEmail() const noexcept = 0;

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