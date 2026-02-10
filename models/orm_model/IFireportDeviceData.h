#pragma once

#include <models/model_interface/IOrmModel.h>
#include <models/request_model/FireportDeviceDataRequest.h>
#include <stduuid/uuid.h>
#include <optional>

namespace request_model {
	struct FireportDeviceDataRequest;
}

namespace drogon_model::defaultdb
{
	class IFireportDeviceData : public model_interface::IOrmModel
	{
	public:
		virtual ~IFireportDeviceData() = default;

		virtual void setId(const std::string& pId) noexcept = 0;
		virtual void setId(std::string&& pId) noexcept = 0;
		virtual void setDeviceId(const std::string& deviceId) noexcept = 0;
		virtual void setDeviceId(std::string&& pDeviceId) noexcept = 0;
		virtual void setType(const int32_t& type) noexcept = 0;
		virtual void setDeviceAddress(const std::string& deviceAddress) noexcept = 0;
		virtual void setDeviceAddress(std::string&& pDeviceAddress) noexcept = 0;
		virtual const std::shared_ptr<std::string>& getId() const noexcept = 0;
		virtual const std::shared_ptr<std::string>& getDeviceId() const noexcept = 0;
		virtual const std::shared_ptr<int32_t>& getType() const noexcept = 0;
		virtual const std::shared_ptr<std::string>& getDeviceAddress() const noexcept = 0;


		void mapToOrmModel(model_interface::IModel& model) const override
		{
			throw std::runtime_error("Mapping to ORM model is not implemented for SensorData ORM model");
		}
		void mapToListModel(model_interface::IModel& model) const override
		{
			throw std::runtime_error("Mapping to list model is not implemented for SensorData ORM model");
		}
		void mapToRequestModel(model_interface::IModel& model) const override
		{
			auto& fireportDataRequestModel = dynamic_cast<request_model::FireportDeviceDataRequest&>(model);
			fireportDataRequestModel.id = getId() ? uuids::uuid().from_string(*getId()) : std::nullopt;
			fireportDataRequestModel.deviceId = getDeviceId() ? *getDeviceId() : std::string();
			fireportDataRequestModel.type = getType() ? std::make_optional(*getType()) : std::nullopt;
			fireportDataRequestModel.deviceAddress = getDeviceAddress() ? *getDeviceAddress() : std::string();
		}
	};

}