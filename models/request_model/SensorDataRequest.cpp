#include <models/orm_model/SensorData.h>

#include "SensorDataRequest.h"

void request_model::SensorDataRequest::mapToOrmModel(model_interface::IModel& ormModel) const
{
	LOG_INFO << "Mapping request model to ORM model";
	auto& sensorDataOrmModel = dynamic_cast<drogon_model::teplomer_db::SensorData&>(ormModel);
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
	if (!date.empty() && !time.empty())
	{
		trantor::Date measureAt;
		measureAt = trantor::Date::fromDbStringLocal(date + " " + time);
		measureAt = measureAt.after(-7200);
		sensorDataOrmModel.setMeasuredAt(measureAt);
	}
}