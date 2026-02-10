#pragma once

#include "IFilterModel.h"

namespace filter_model
{
	class FireportDeviceFilter : public IFilterModel
	{
	public:
		std::string deviceId;
		std::optional<int32_t> type = std::nullopt;
		std::string deviceAddress;
		std::string orderBy = "deviceId";
		std::string orderDirection = "DESC";
		int limit = 20;
		int offset = 1;
		FireportDeviceFilter() = default;

		Json::Value toJson() const
		{
			Json::Value json;
			if (!deviceId.empty())
				json["deviceId"] = deviceId;
			if (type.has_value())
				json["type"] = *type;
			if (!orderBy.empty())
				json["orderBy"] = orderBy;
			if (!orderDirection.empty())
				json["orderDirection"] = orderDirection;
			if (!deviceAddress.empty())
				json["deviceAddress"] = deviceAddress;
			json["limit"] = limit;
			json["offset"] = offset;
			return json;
		}

	};
}

namespace drogon
{
	template <>
	inline filter_model::FireportDeviceFilter fromRequest<filter_model::FireportDeviceFilter>(const HttpRequest& req)
	{
		filter_model::FireportDeviceFilter filter;
		try
		{
			auto json = req.getJsonObject();
			if (json)
			{
				if (json->isMember("deviceId"))
					filter.deviceId = (*json)["deviceId"].asString();
				if (json->isMember("type"))
					filter.type = (*json)["type"].asInt();
				if (json->isMember("orderBy"))
					filter.orderBy = (*json)["orderBy"].asString();
				if (json->isMember("orderDirection"))
					filter.orderDirection = (*json)["orderDirection"].asString();
				if (json->isMember("deviceAddress"))
					filter.deviceAddress = (*json)["deviceAddress"].asString();
				if (json->isMember("limit"))
					filter.limit = (*json)["limit"].asInt();
				if (json->isMember("offset"))
					filter.offset = (*json)["offset"].asInt();
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "Error parsing JSON: " << e.what();
			throw;
		}
		return filter;
	}
}