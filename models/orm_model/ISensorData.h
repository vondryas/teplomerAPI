#pragma once

#include <models/model_interface/IOrmModel.h>
#include <models/request_model/SensorDataRequest.h>
#include <stduuid/uuid.h>
#include <optional>

namespace request_model {
	struct SensorDataRequest;
}

namespace drogon_model::teplomer_db
{
	class ISensorData : public model_interface::IOrmModel
	{
	public:
		virtual ~ISensorData() = default;

		virtual void setId(const std::string& pId) noexcept = 0;
		virtual void setId(std::string&& pId) noexcept = 0;
		virtual void setBattery(const int32_t& battery) noexcept = 0;
		virtual void setTemperature(const double& temperature) noexcept = 0;
		virtual void setHumidity(const double& humidity) noexcept = 0;
		virtual void setPressure(const double& pressure) noexcept = 0;
		virtual void setMeasuredAt(const trantor::Date& date) noexcept = 0;
		virtual const std::shared_ptr<std::string>& getId() const noexcept = 0;
		virtual const std::shared_ptr<int32_t>& getBattery() const noexcept = 0;
		virtual const std::shared_ptr<double>& getTemperature() const noexcept = 0;
		virtual const std::shared_ptr<double>& getHumidity() const noexcept = 0;
		virtual const std::shared_ptr<double>& getPressure() const noexcept = 0;
		virtual const std::shared_ptr<trantor::Date>& getMeasuredAt() const noexcept = 0;

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
			auto& sensorDataRequestModel = dynamic_cast<request_model::SensorDataRequest&>(model);
			sensorDataRequestModel.id = getId() ? uuids::uuid().from_string(*getId()) : std::nullopt;
			sensorDataRequestModel.battery = getBattery() ? std::make_optional<int32_t>(*getBattery()) : std::nullopt;
			sensorDataRequestModel.temperature = getTemperature() ? std::make_optional<double>(*getTemperature()) : std::nullopt;
			sensorDataRequestModel.humidity = getHumidity() ? std::make_optional<double>(*getHumidity()) : std::nullopt;
			sensorDataRequestModel.pressure = getPressure() ? std::make_optional<double>(*getPressure()) : std::nullopt;
			if (getMeasuredAt())
			{
				sensorDataRequestModel.date = getMeasuredAt()->toDbString().substr(0, 10);
				sensorDataRequestModel.time = getMeasuredAt()->toDbString().substr(11, 8);
			}
			else
			{
				sensorDataRequestModel.date.clear();
				sensorDataRequestModel.time.clear();
			}
		}
	};

}
