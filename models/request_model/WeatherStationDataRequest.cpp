#include "WeatherStationDataRequest.h"
#include <models/orm_model/WeatherStationData.h>


void request_model::WeatherStationDataRequest::mapToOrmModel(model_interface::IModel& ormModel) const
{
	LOG_INFO << "Mapping request model to ORM model";
	auto& sensorDataOrmModel = dynamic_cast<WeatherStationData&>(ormModel);
	if (id.has_value())
		sensorDataOrmModel.setId(uuids::to_string(*id));
	if (battery.has_value())
		sensorDataOrmModel.setBattery(*battery);
	if (temperature.has_value())
		sensorDataOrmModel.setTemperature(*temperature);
	if (humidity.has_value())
		sensorDataOrmModel.setHumidity(*humidity);
	if (pressure.has_value())
		sensorDataOrmModel.setPressure(*pressure);
	if (!deviceId.empty())
	{
		sensorDataOrmModel.setDeviceId(deviceId);
	}

	if (!date.empty() && !time.empty())
	{
		trantor::Date measureAt;
		measureAt = trantor::Date::fromDbStringLocal(date + " " + time);
		measureAt = measureAt.after(-7200);
		sensorDataOrmModel.setMeasureAt(measureAt);
	}
}