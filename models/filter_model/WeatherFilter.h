#pragma once

#include "IFilterModel.h"

namespace filter_model
{
	struct WeatherFilter : public IFilterModel
	{
		std::string stationId;
		std::string startDate;
		std::string endDate;
		std::string orderBy = "measure_at";
		std::string orderDirection = "DESC"; // Default order direction
		int limit = 20;
		int offset = 1;
		WeatherFilter() = default;
		WeatherFilter(const std::string& stationId, const std::string& startDate, const std::string& endDate,
			const std::string& orderBy, int limit, int offset)
			: stationId(stationId), startDate(startDate), endDate(endDate), orderBy(orderBy), limit(limit), offset(offset) {
		}
		Json::Value toJson() const
		{
			Json::Value json;
			if (!stationId.empty())
			{
				json["stationId"] = stationId;
			}
			if (!startDate.empty())
			{
				json["startDate"] = startDate;
			}
			if (!endDate.empty())
			{
				json["endDate"] = endDate;
			}
			if (!orderBy.empty())
			{
				json["orderBy"] = orderBy;
			}
			return json;
		}

	};
}

namespace drogon
{
	template <>
	inline filter_model::WeatherFilter fromRequest<filter_model::WeatherFilter>(const HttpRequest& req)
	{
		LOG_INFO << "parse request";
		auto json = req.getJsonObject();
		filter_model::WeatherFilter data;
		LOG_INFO << (json->empty() ? "Request JSON is empty" : "Request JSON is not empty");
		if (json)
		{
			LOG_INFO << "Received JSON: " << json->toStyledString();
			data.stationId = (*json)["stationId"].isNull() ? "" : (*json)["stationId"].asString();
			data.startDate = (*json)["startDate"].isNull() ? "" : (*json)["startDate"].asString();
			data.endDate = (*json)["endDate"].isNull() ? "" : (*json)["endDate"].asString();
			data.orderBy = (*json)["orderBy"].isNull() ? "measure_at" : (*json)["orderBy"].asString();
			data.orderDirection = (*json)["orderDirection"].isNull() ? "DESC" : (*json)["orderDirection"].asString();
			data.limit = (*json)["limit"].isNull() ? 20 : (*json)["limit"].asInt();
			data.offset = (*json)["offset"].isNull() ? 1 : (*json)["offset"].asInt();
		}
		return data;
	}
}