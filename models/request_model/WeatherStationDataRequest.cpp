#include "WeatherStationDataRequest.h"
#include <models/orm_model/WeatherStationData.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>


std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

void request_model::WeatherStationDataRequest::correctDate()
{
	// split date string
	std::vector<std::string> dateParts = split(date, '-');
	// if date is in format YYYY-MM-DD do nothing
	// if date is in format DD-MM-YYYY convert to YYYY-MM-DD
	if (dateParts.size() == 3)
	{
		if (dateParts[0].size() == 2 && dateParts[2].size() >= 4)
		{
			std::string newDate = dateParts[2] + "-" + dateParts[1] + "-" + dateParts[0];
			date = newDate;
		}
	}
}

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