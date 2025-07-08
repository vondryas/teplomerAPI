#pragma once

#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <drogon/HttpRequest.h>
#include <json/json.h>
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <tuple>
#include <stdint.h>
#include <iostream>

namespace request_model
{
	struct SensorData
	{
		int32_t id;
		int32_t battery;
		std::string temperature;
		std::string humidity;
		std::string pressure;
		trantor::Date date;
		std::string time;
		SensorData();
		bool isEmpty();
	};
}

namespace drogon
{
	template <>
	inline request_model::SensorData fromRequest(const HttpRequest& req)
	{
		LOG_INFO << "parse request";
		auto json = req.getJsonObject();
		request_model::SensorData data;
		if (json)
		{
			LOG_INFO << "Received JSON: " << json->toStyledString();
			data.id = (*json)["id"].asInt();
			data.battery = (*json)["battery"].asInt();
			data.temperature = (*json)["temperature"].asString();
			data.humidity = (*json)["humidity"].asString();
			data.pressure = (*json)["pressure"].asString();
			data.date = trantor::Date::fromDbStringLocal((*json)["date"].asString());
			data.time = (*json)["time"].asString();
		}
		return data;
	}
}